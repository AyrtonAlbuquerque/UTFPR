namespace AuctionWebService.DTO
{
    public class ItemDTO
    {
        public int Id { get; set; }
        public int Owner { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public decimal Value { get; set; }
        public DateTime Expiration { get; set; }
        public IList<int> Biders { get; set; }
    }
}