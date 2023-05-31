using AuctionWebService.DTO;

namespace AuctionWebService.Services.Interfaces
{
    public interface IAuthService
    {
        Task<LoginResponseDTO> Login(LoginDTO client);
    }
}