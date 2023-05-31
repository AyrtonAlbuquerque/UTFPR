using AuctionWebService.Model;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;

namespace AuctionWebService.Data
{
    public class DataContext : DbContext
    {
        private string _connectionString;

        public DataContext(DbContextOptions<DataContext> options) : base(options)
        {
        }

        public DataContext(DbContextOptions<DataContext> options, string connectionString)
        {
            _connectionString = connectionString;
        }

        protected override void OnConfiguring(DbContextOptionsBuilder options)
        {
            if (!string.IsNullOrEmpty(_connectionString))
                options.UseSqlServer(_connectionString);
            options.UseLazyLoadingProxies();
        }

        protected override void OnModelCreating(ModelBuilder builder)
        {
            base.OnModelCreating(builder);

            builder.Entity<Item>()
                .HasOne(i => i.Owner)
                .WithMany(c => c.Items)
                .HasForeignKey(i => i.OwnerId)
                .IsRequired();

            builder.Entity<Bider>()
                .HasOne(b => b.Client)
                .WithMany(c => c.Biders)
                .HasForeignKey(b => b.ClientId)
                .IsRequired();

            builder.Entity<Bider>()
                .HasOne(b => b.Item)
                .WithMany(i => i.Biders)
                .HasForeignKey(b => b.ItemId)
                .IsRequired();

            builder.Entity<Bid>()
                .HasOne(c => c.Client)
                .WithMany(b => b.Bids)
                .HasForeignKey(b => b.ClientId)
                .IsRequired();

            builder.Entity<Bid>()
                .HasOne(i => i.Item)
                .WithMany(b => b.Bids)
                .HasForeignKey(b => b.ItemId)
                .IsRequired();
        }

        public DbSet<Client> Client => Set<Client>();
        public DbSet<Item> Item => Set<Item>();
        public DbSet<Bid> Bid => Set<Bid>();
        public DbSet<Bider> Bider => Set<Bider>();
    }
}