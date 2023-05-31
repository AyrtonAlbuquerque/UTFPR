namespace AuctionWebService.Model
{
    public class Bid
    {
        public virtual int Id { get; protected set; }
        public virtual decimal Value { get; set; }
        public virtual string Signature { get; set; }
        public virtual DateTime Date { get; set; }
        public virtual int ClientId { get; set; }
        public virtual int ItemId { get; set; }

        // Relationships
        public virtual Client Client { get; set; }
        public virtual Item Item { get; set; }
    }
}