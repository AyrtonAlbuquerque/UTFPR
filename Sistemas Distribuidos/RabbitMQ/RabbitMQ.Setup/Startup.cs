using RabbitMQ.Client;
using RabbitMQ.Exchanges;
using RabbitMQ.Queues;
using RabbitMQ.Bindings;

namespace RabbitMQ.Setup
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
            _channel.ExchangeDeclare(Exchange.Direct, "direct", true, false, null);
        }

        private void CreateQueues()
        {
            _channel.QueueDeclare(Queue.Information, true, false, false, null);
            _channel.QueueDeclare(Queue.Warning, true, false, false, null);
            _channel.QueueDeclare(Queue.Error, true, false, false, null);
            _channel.QueueBind(Queue.Information, Exchange.Direct, Binding.Information);
            _channel.QueueBind(Queue.Warning, Exchange.Direct, Binding.Warning);
            _channel.QueueBind(Queue.Error, Exchange.Direct, Binding.Error);
        }

        private void DeleteExchanges()
        {
            _channel.ExchangeDelete(Exchange.Direct);
        }

        private void DeleteQueues()
        {
            _channel.QueueDelete(Queue.Information);
            _channel.QueueDelete(Queue.Warning);
            _channel.QueueDelete(Queue.Error);
        }
    }
}