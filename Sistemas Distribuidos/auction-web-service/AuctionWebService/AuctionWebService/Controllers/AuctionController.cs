using AuctionWebService.DTO;
using AuctionWebService.Model;
using AuctionWebService.Services.Interfaces;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;

namespace AuctionWebService.Controllers
{
    [Authorize]
    [ApiController]
    [Route("api/[controller]")]
    public class AuctionController : Controller
    {
        private readonly IAuctionService _auctionService;

        public AuctionController(IAuctionService auctionService)
        {
            _auctionService = auctionService;
        }

        [HttpPost("bid")]
        public async Task<ActionResult<string>> Post(BidDTO bid)
        {
            try
            {
                return Ok(await _auctionService.Bid(bid));
            }
            catch (Exception e)
            {
                return BadRequest(e.Message);
            }
        }

        [HttpGet("list")]
        public async Task<ActionResult<List<ItemDTO>>> Get()
        {
            try
            {
                return Ok(await _auctionService.List());
            }
            catch (Exception e)
            {
                return BadRequest(e.Message);
            }
        }

        [HttpPost("register")]
        public async Task<ActionResult<ItemDTO>> Post(RegisterDTO item)
        {
            try
            {
                return Ok(await _auctionService.Register(item));
            }
            catch (Exception e)
            {
                return BadRequest(e.Message);
            }
        }

        [HttpPatch("subscribe")]
        public async Task<ActionResult<string>> Patch(SubscribeDTO subscription)
        {
            try
            {
                return Ok(await _auctionService.Subscribe(subscription));
            }
            catch (Exception e)
            {
                return BadRequest(e.Message);
            }
        }
    }
}