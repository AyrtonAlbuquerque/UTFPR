using AuctionWebService.DTO;
using AuctionWebService.Model;
using AutoMapper;

namespace AuctionWebService.AutoMapper
{
    public class AutoMapperProfile : Profile
    {
        public AutoMapperProfile()
        {
            // CreateMap<Bid, BidDTO>()
            //     .ForMember(dest => dest.Item.Id,
            //         opt => opt.MapFrom(src => src.Item.Id));
            // CreateMap<Bid, BidDTO>().ReverseMap();
        }
    }
}