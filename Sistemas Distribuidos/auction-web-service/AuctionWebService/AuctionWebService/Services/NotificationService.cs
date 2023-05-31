using AuctionWebService.Services.Interfaces;
using Lib.AspNetCore.ServerSentEvents;

namespace AuctionWebService.Services
{
    public class NotificationService : INotificationService
    {
        private readonly IServerSentEventsService _client;

        public NotificationService(IServerSentEventsService client)
        {
            _client = client;
        }

        public void Notify(int id, string message)
        {
            var clients = _client.GetClients();

            if (clients.Any())
            {
                _client.SendEventAsync(new ServerSentEvent
                {
                    Id = id.ToString(),
                    Data = new List<string>() { message }
                });
            }
        }

        public async Task NotifyAsync(int id, string message)
        {
            var clients = _client.GetClients();

            if (clients.Any())
            {
                await _client.SendEventAsync(new ServerSentEvent
                {
                    Id = id.ToString(),
                    Data = new List<string>() { message }
                });
            }
        }
    }
}