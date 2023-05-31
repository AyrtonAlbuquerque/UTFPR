using AuctionWebService.Data;
using AuctionWebService.DTO;
using AuctionWebService.Model;
using AuctionWebService.Services.Interfaces;
using AutoMapper;
using Microsoft.EntityFrameworkCore;
using System.Security.Claims;
using Microsoft.IdentityModel.Tokens;
using System.IdentityModel.Tokens.Jwt;
using System.Text;

namespace AuctionWebService.Services
{
    public class AuthService : IAuthService
    {
        private readonly DataContext _context;
        private readonly IConfiguration _configuration;
        private readonly INotificationService _notificationService;

        public AuthService(DataContext context, IConfiguration configuration, INotificationService notificationService)
        {
            _context = context;
            _configuration = configuration;
            _notificationService = notificationService;
        }

        public async Task<LoginResponseDTO> Login(LoginDTO login)
        {
            var clients = await _context.Client.ToListAsync();
            var client = new Client()
            {
                Name = login.Name,
                PublicKey = login.PublicKey
            };

            foreach (var user in clients)
            {
                await _notificationService.NotifyAsync(user.Id, $"{client.Name} has entered the auction");
            }

            _context.Client.Add(client);
            await _context.SaveChangesAsync();

            return new LoginResponseDTO
            {
                Id = client.Id,
                Type = "Bearer",
                Token = GenerateToken(client)
            };
        }

        private string GenerateToken(Client client)
        {
            var handler = new JwtSecurityTokenHandler();
            var token = handler.CreateToken(new SecurityTokenDescriptor
            {
                Subject = new ClaimsIdentity(new List<Claim>
                {
                    new Claim(ClaimTypes.NameIdentifier, client.Id.ToString()),
                    new Claim(ClaimTypes.Name, client.Name)
                }),
                Expires = DateTime.Now.AddDays(1),
                SigningCredentials = new SigningCredentials(new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_configuration["AppSettings:Token"])), SecurityAlgorithms.HmacSha512Signature)
            });

            return handler.WriteToken(token);
        }
    }
}