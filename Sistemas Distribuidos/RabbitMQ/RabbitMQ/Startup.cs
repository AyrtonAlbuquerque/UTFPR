using RabbitMQ.Client;
using RabbitMQ.Exchanges;
using RabbitMQ.Queues;

namespace RabbitMQ
{
    public class Startup : IDisposable
    {
        private IConnection _connection;
        private IModel _channel;

        public Startup(string host, int port)
        {
            if (string.IsNullOrEmpty(host)) throw new Exception("O IP do host precisa ser informado");
            if (port < 0 || port > 65535) throw new Exception($"O valor da porta {port} é inválido");

            var factory = new ConnectionFactory
            {
                HostName = host,
                VirtualHost = "/",
                Port = port,
                UserName = "guest",
                Password = "guest"
            };
            _connection = factory.CreateConnection();
            _channel = _connection.CreateModel();

            CreateExchanges();
            CreateQueues();
        }

        public void Dispose()
        {
            DeleteExchanges();
            DeleteQueues();
            _channel.Close();
            _connection.Close();
        }

        private void CreateExchanges()
        {
            _channel.ExchangeDeclare(Exchange.Fanout, "fanout", true, false, null);
        }

        private void CreateQueues()
        {
            _channel.QueueDeclare(Queue.First, true, false, false, null);
            _channel.QueueDeclare(Queue.Second, true, false, false, null);
            _channel.QueueBind(Queue.First, Exchange.Fanout, "");
            _channel.QueueBind(Queue.Second, Exchange.Fanout, "");
        }

        private void DeleteExchanges()
        {
            _channel.ExchangeDelete(Exchange.Fanout);
        }

        private void DeleteQueues()
        {
            _channel.QueueDelete(Queue.First);
            _channel.QueueDelete(Queue.Second);
        }
    }
}