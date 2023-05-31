namespace AuctionWebService.Model
{
    public class Bider
    {
        public virtual int Id { get; protected set; }
        public virtual int ClientId { get; set; }
        public virtual int ItemId { get; set; }

        // Relationships
        public virtual Client Client { get; set; }
        public virtual Item Item { get; set; }
    }
}