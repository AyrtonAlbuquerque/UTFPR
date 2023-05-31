namespace AuctionWebService.DTO
{
    public class BidDTO
    {
        public int ClientId { get; set; }
        public int ItemId { get; set; }
        public decimal Value { get; set; }
        public string Signature { get; set; }
    }
}