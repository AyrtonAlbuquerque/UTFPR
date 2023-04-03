namespace RabbitMQ
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var setup = new Startup("localhost", 5672))
            {
                Console.WriteLine("Setup successful, press any key to terminate...");
                Console.ReadLine();
            }
        }
    }
}