namespace AuctionWebService.DTO
{
    public class RegisterDTO
    {
        public int ClientId { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public decimal Value { get; set; }
        public DateTime Expiration { get; set; }
    }
}