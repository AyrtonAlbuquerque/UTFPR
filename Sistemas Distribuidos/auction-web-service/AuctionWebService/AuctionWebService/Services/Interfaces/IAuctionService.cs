using AuctionWebService.DTO;
using Microsoft.AspNetCore.Mvc;

namespace AuctionWebService.Services.Interfaces
{
    public interface IAuctionService
    {
        Task<string> Bid(BidDTO bid);
        Task<List<ItemDTO>> List();
        Task<ItemDTO> Register(RegisterDTO item);
        Task<string> Subscribe(SubscribeDTO subscription);
    }
}