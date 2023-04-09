using RabbitMQ;

namespace Setup
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            var setup = new Startup("localhost", 5672);

            try
            {
                Console.WriteLine("Setup successful, press any key to terminate...");
                Console.ReadLine();
            }
            finally
            {
                setup.Dispose();
            }
        }
    }
}