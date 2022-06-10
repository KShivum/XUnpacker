namespace XUnpackerCore
{
    class XFileHandler
    {
        public static void UnPack(string filePath, string outputDirectory)
        {
            UInt32 packId, verId, dirCount, dirOffset, fileCount, fileOffset;
            Byte[] emptySpace = new Byte[32];

            BinaryReader baseFile = new BinaryReader(File.Open(filePath, FileMode.Open));
            //Reads 4 bytes and stores into the header variables
            packId = baseFile.ReadUInt32();
            verId = baseFile.ReadUInt32();
            dirCount = baseFile.ReadUInt32();
            dirOffset = baseFile.ReadUInt32();
            fileCount = baseFile.ReadUInt32();
            fileOffset = baseFile.ReadUInt32();



            //Create Directories
            int currentDirectories = 0;
            int directoryOffset = 0;
            Dictionary<int, string> directoryMap = new Dictionary<int, string>();

            directoryMap.Add(0, outputDirectory);

            while (currentDirectories < dirCount)
            {
                baseFile.BaseStream.Seek(dirOffset + directoryOffset, SeekOrigin.Begin);
                
                byte dirNameLength = baseFile.ReadByte();

                byte[] dirName = new byte[dirNameLength];
            }
        }
    }
}