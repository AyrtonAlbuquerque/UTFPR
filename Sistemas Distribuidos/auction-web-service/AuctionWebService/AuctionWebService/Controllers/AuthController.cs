using AuctionWebService.DTO;
using AuctionWebService.Services.Interfaces;
using Microsoft.AspNetCore.Mvc;

namespace AuctionWebService.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class AuthController : Controller
    {
        private readonly IAuthService _authService;

        public AuthController(IAuthService authService)
        {
            _authService = authService;
        }

        [HttpPost]
        public async Task<ActionResult<LoginResponseDTO>> Post(LoginDTO login)
        {
            try
            {
                return Ok(await _authService.Login(login));
            }
            catch (Exception e)
            {
                return BadRequest(e.Message);
            }
        }
    }
}