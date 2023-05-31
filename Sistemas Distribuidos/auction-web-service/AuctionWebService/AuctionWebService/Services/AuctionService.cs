using AuctionWebService.Data;
using AuctionWebService.DTO;
using AuctionWebService.Model;
using AuctionWebService.Services.Interfaces;
using AutoMapper;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using System.Threading.Tasks;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Timers;
using System.Text.Json.Serialization;
using Timer = System.Timers.Timer;

namespace AuctionWebService.Services
{
    public class AuctionService : IAuctionService
    {
        private readonly DataContext _context;
        private readonly IConfiguration _configuration;
        private readonly INotificationService _notificationService;
        private readonly RSACryptoServiceProvider _rsa;

        public AuctionService(DataContext context, IConfiguration configuration, INotificationService notificationService)
        {
            _context = context;
            _configuration = configuration;
            _notificationService = notificationService;
            _rsa = new RSACryptoServiceProvider(2048);
        }

        public async Task<string> Bid(BidDTO bid)
        {
            var item = await _context.Item.FirstOrDefaultAsync(i => i.Id == bid.ItemId);
            var client = await _context.Client.FirstOrDefaultAsync(c => c.Id == bid.ClientId);
            // var data = Encoding.ASCII.GetBytes($"{bid.ItemId}-{bid.Value}");
            // var signature = Convert.FromBase64String(bid.Signature);
            // _rsa.ImportRSAPublicKey(Convert.FromBase64String(client.PublicKey), out _);

            if (item is null) return "Bid refused, item not found.";
            if (client is null) return "Bid refused, client not found.";
            if (!item.Active) return "Bid refused, auction is closed for this item.";
            if (bid.Value <= item.Value) return "Bid refused, value is lower than current highest bid.";
            // if (!_rsa.VerifyData(data, signature, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1)) return "Bid refused, invalid signature.";

            item.OwnerId = client.Id;
            item.Value = bid.Value;

            foreach (var bider in item.Biders)
            {
                if (bider.ClientId != client.Id)
                {
                    await _notificationService.NotifyAsync(bider.ClientId, $"{client.Name} now has the highest bid on item ({item.Id}): {item.Name} for: R$ {item.Value}.");
                }
            }

            if (!item.Biders.Any(b => b.Client.Id == client.Id))
            {
                item.Biders.Add(new Bider
                {
                    ClientId = client.Id,
                    ItemId = item.Id
                });
            }

            _context.Bid.Add(new Bid
            {
                ItemId = item.Id,
                ClientId = client.Id,
                Value = bid.Value,
                Signature = bid.Signature.ToString(),
                Date = DateTime.Now
            });
            await _context.SaveChangesAsync();

            return "Bid accepted.";
        }

        public async Task<List<ItemDTO>> List()
        {
            var items = await _context.Item.Where(i => i.Active == true).ToListAsync();
            var response = new List<ItemDTO>();

            foreach (var item in items)
            {
                var i = new ItemDTO
                {
                    Id = item.Id,
                    Owner = item.Owner.Id,
                    Name = item.Name,
                    Description = item.Description,
                    Value = item.Value,
                    Expiration = item.Expiration,
                    Biders = new List<int>()
                };

                foreach (var bider in item.Biders)
                {
                    i.Biders.Add(bider.Client.Id);
                }

                response.Add(i);
            }

            return response;
        }

        public async Task<ItemDTO> Register(RegisterDTO item)
        {
            var owner = await _context.Client.FirstOrDefaultAsync(c => c.Id == item.ClientId);
            var newItem = new Item
            {
                OwnerId = item.ClientId,
                Name = item.Name,
                Description = item.Description,
                Value = item.Value,
                Expiration = item.Expiration,
                Active = DateTime.Now < item.Expiration,
                Biders = new List<Bider>()
            };

            if (newItem.Active)
            {
                var timer = new Timer((item.Expiration - DateTime.Now).TotalMilliseconds);
                var clients = await _context.Client.ToListAsync();

                _context.Item.Add(newItem);
                await _context.SaveChangesAsync();

                newItem.Biders.Add(new Bider
                {
                    ClientId = item.ClientId,
                    ItemId = newItem.Id
                });

                await _context.SaveChangesAsync();

                foreach (var client in clients)
                {
                    await _notificationService.NotifyAsync(client.Id, $"A new item is up for auction ({newItem.Id}): {newItem.Name} - R$ {newItem.Value}. Expires at: {newItem.Expiration}.");
                }

                timer.Elapsed += (sender, e) => OnExpire(sender, e, newItem.Id, _configuration.GetConnectionString("DefaultConnection"));
                timer.AutoReset = false;
                timer.Enabled = true;

                return new ItemDTO
                {
                    Id = newItem.Id,
                    Owner = newItem.Owner.Id,
                    Name = newItem.Name,
                    Description = newItem.Description,
                    Value = newItem.Value,
                    Expiration = newItem.Expiration,
                    Biders = new List<int> { newItem.Owner.Id }
                };
            }
            else
                throw new Exception($"Failed to register item. The item is not active.");
        }

        public async Task<string> Subscribe(SubscribeDTO subscription)
        {
            var item = await _context.Item.FirstOrDefaultAsync(i => i.Id == subscription.ItemId);
            var client = await _context.Client.FirstOrDefaultAsync(c => c.Id == subscription.ClientId);

            if (item is null) return "Subscription failed, Item not found.";
            if (client is null) return "Subscription failed, Client not found.";
            if (!item.Active) return "Subscription failed, auction is closed for this item.";

            if (!item.Biders.Any(b => b.Client.Id == client.Id))
            {
                item.Biders.Add(new Bider
                {
                    ClientId = client.Id,
                    ItemId = item.Id
                });
                await _context.SaveChangesAsync();

                return $"Successfully subscribed to item ({item.Id}): {item.Name}";
            }
            else
                return "Subscription failed, you are already subscribed to this item.";
        }

        private void OnExpire(object sender, ElapsedEventArgs e, int id, string connectionString)
        {
            using (var context = new DataContext(null, connectionString))
            {
                var item = context.Item.FirstOrDefault(i => i.Id == id);
                var clients = context.Client.ToList();

                item.Active = false;
                context.SaveChanges();

                foreach (var client in clients)
                {
                    _notificationService.Notify(client.Id, $"Auction closed for item ({item.Id}): {item.Name}. {item.Owner.Name} won with a bid of: R$ {item.Value}.");
                }
            }
        }
    }
}