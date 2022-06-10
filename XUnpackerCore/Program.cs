namespace XUnpackerCore
{
    class Program
    {
        static void Main(string[] args)
        {
            //Temporary hardcode
            //XFileHandler.Pack("Test/", "OutTest/base.x");
            XFileHandler.UnPack("OutTest/base.x", "Test2/");
        }
    }
}