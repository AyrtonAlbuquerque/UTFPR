namespace AuctionWebService.Services.Interfaces
{
    public interface INotificationService
    {
        void Notify(int id, string message);
        Task NotifyAsync(int id, string message);
    }
}