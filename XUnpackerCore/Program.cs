namespace XUnpackerCore
{
    class Program
    {
        static void Main(string[] args)
        {
            if(args.Length == 0)
            {
                Console.WriteLine("Please provide a file to unpack.");
                return;
            }
            if(args[0].Equals("P"))
            {
                XFileHandler.Pack(args[1], args[2]);
            }
            else if(args[0].Equals("U"))
            {
                XFileHandler.UnPack(args[1], args[2]);
            }
            else
            {
                Console.WriteLine("Please provide a valid command.");
            }
        }
    }
}