namespace AuctionWebService.Model
{
    public class Client
    {
        public int Id { get; protected set; }
        public string Name { get; set; }
        public string PublicKey { get; set; }

        // Relationships
        public virtual IList<Item> Items { get; set; }
        public virtual IList<Bider> Biders { get; set; }
        public virtual IList<Bid> Bids { get; set; }
    }
}