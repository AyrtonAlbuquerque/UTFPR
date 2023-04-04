using RabbitMQ.Queues;

namespace RabbitMQ.Consumer
{
    class Program
    {
        static void Main(string[] args)
        {
            var selected = false;
            var option = 1;

            using (var consumer = new Consumer("localhost", 5672))
            {
                Console.WriteLine("Choose a topic to subscribe to: ");
                (int left, int top) = Console.GetCursorPosition();

                while (!selected)
                {
                    Console.SetCursorPosition(left, top);
                    Console.WriteLine($"{(option == 1 ? ">   " : "    ")}Information Log");
                    Console.WriteLine($"{(option == 2 ? ">   " : "    ")}Warning Log");
                    Console.WriteLine($"{(option == 3 ? ">   " : "    ")}Error Log");

                    switch (Console.ReadKey(true).Key)
                    {
                        case ConsoleKey.UpArrow:
                            option = option == 1 ? 3 : --option;
                            break;

                        case ConsoleKey.DownArrow:
                            option = option == 3 ? 1 : ++option;
                            break;

                        case ConsoleKey.Enter:
                            selected = true;
                            break;
                    }
                }

                switch (option)
                {
                    case 1:
                        consumer.Subscribe(Queue.Information);
                        break;
                    case 2:
                        consumer.Subscribe(Queue.Warning);
                        break;
                    case 3:
                        consumer.Subscribe(Queue.Error);
                        break;
                }

                Console.WriteLine("Listening... Press any key to exit");
                Console.ReadLine();
                Console.WriteLine("Consumer termianted");
            }
        }
    }
}