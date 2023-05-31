using System;
using System.Collections.Generic;

namespace AuctionWebService.Model
{
    public class Item
    {
        public int Id { get; protected set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public decimal Value { get; set; }
        public DateTime Expiration { get; set; }
        public bool Active { get; set; }
        public int OwnerId { get; set; }

        // Relationships
        public virtual Client Owner { get; set; }
        public virtual IList<Bid> Bids { get; set; }
        public virtual IList<Bider> Biders { get; set; }
    }
}