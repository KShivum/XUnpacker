using System.Text;

namespace XUnpackerCore
{
    class XFileHandler
    {
        private class FileInfo
        {
            public UInt32 dirIndex;
            public byte fileNameSize;
            public string fileName;
            public UInt32 dataOffset;
            public UInt32 dataSize;
        }

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
                dirName = baseFile.ReadBytes(dirNameLength);
                string dirNameString = Encoding.ASCII.GetString(dirName);

                dirNameString = dirNameString.Replace("\\", "/");
                System.Console.WriteLine($"Creating directory: {dirNameString}");
                directoryOffset += dirNameLength + 1;

                directoryMap.Add(currentDirectories + 1, Path.Combine(outputDirectory, dirNameString));
                currentDirectories++;

                Directory.CreateDirectory(Path.Combine(outputDirectory, dirNameString));

            }

            //Create Files
            System.Console.WriteLine("\n\n------------------Creating Files------------------");
            System.Console.WriteLine($"Creating {fileCount} files");

            int currentFiles = 0;
            int fileCurrentOffset = 0;
            while (currentFiles < fileCount)
            {
                baseFile.BaseStream.Seek(fileOffset + fileCurrentOffset, SeekOrigin.Begin);

                UInt32 dirIndex, dataOffset, dataSize;
                byte fileSize;

                dirIndex = baseFile.ReadUInt32();
                fileSize = baseFile.ReadByte();
                byte[] fileName = new byte[fileSize];
                fileName = baseFile.ReadBytes(fileSize);
                dataOffset = baseFile.ReadUInt32();
                dataSize = baseFile.ReadUInt32();

                baseFile.BaseStream.Seek(dataOffset, SeekOrigin.Begin);
                byte[] fileData = new byte[dataSize];
                fileData = baseFile.ReadBytes((int)dataSize);

                string fileNameString = Encoding.ASCII.GetString(fileName);

                string dirName = directoryMap[(int)dirIndex];

                BinaryWriter fileWriter = new BinaryWriter(File.Open(Path.Combine(dirName, fileNameString), FileMode.Create));
                System.Console.WriteLine($"Creating file: {fileNameString}");
                fileWriter.Write(fileData);
                fileWriter.Close();
                currentFiles++;
                fileCurrentOffset += fileSize + 4 + 1 + 4 + 4;
            }
        }

        public static void Pack(string directory, string? output = null)
        {
            if (output == null)
            {
                output = Path.Combine(directory, "base.x");
            }
            Dictionary<int, string> directories = new Dictionary<int, string>();
            DirectoryInfo inDir = new DirectoryInfo(directory);
            directory = inDir.FullName;

            if(directory.EndsWith("\\") || directory.EndsWith("/"))
            {
                directory = directory.Substring(0, directory.Length - 1);
            }

            directories.Add(0, directory);
            List<FileInfo> files = new List<FileInfo>();
            int dirIndex = 1;
            int fileSize = 0;

            System.Console.WriteLine("------------------Scanning Files and Directories------------------");
            DirectoryInfo dir = new DirectoryInfo(directory);
            var allDirectories = dir.GetDirectories("*", SearchOption.AllDirectories);
            foreach (var d in allDirectories)
            {
                directories.Add(dirIndex, d.FullName);
                dirIndex++;
            }

            //Get all files
            var allFiles = dir.GetFiles("*", SearchOption.AllDirectories);
            foreach (var f in allFiles)
            {
                if (f.Name.Equals(System.AppDomain.CurrentDomain.FriendlyName) || f.Name.Equals("base.x"))
                {
                    System.Console.WriteLine("Skipping file: " + f.Name);
                    continue;
                }
                FileInfo file = new FileInfo();
                System.Console.WriteLine($"Found file: {f.FullName}");
                file.fileName = f.Name;

                file.dirIndex = (UInt32)directories.First(x => x.Value.Equals(f.DirectoryName)).Key;

                file.fileNameSize = (byte)file.fileName.Length;
                files.Add(file);
                fileSize += file.fileNameSize + 1 + 4 + 4 + 4;

            }

            List<Byte> fullFileData = new List<byte>();
            int currentOffset = 24 + 32;
            System.Console.WriteLine("------------------Reading File Data------------------");
            //Read files and store data into list

            for (int i = 0; i < files.Count; i++)
            {
                System.Console.WriteLine($"Reading file: {files[i].fileName}");
                BinaryReader file;
                string fileDirectory = directories[(int)files[i].dirIndex];
                file = new BinaryReader(File.Open(Path.Combine(fileDirectory, files[i].fileName), FileMode.Open));
                files[i].dataSize = (UInt32)file.BaseStream.Length;
                files[i].dataOffset = (UInt32)currentOffset;
                byte[] fileData = new byte[files[i].dataSize];
                fileData = file.ReadBytes((int)files[i].dataSize);
                fullFileData.AddRange(fileData);
                currentOffset += (int)files[i].dataSize;
                file.Close();
            }

            //Create header
            UInt32 packId = (UInt32)BitConverter.ToInt32(Encoding.ASCII.GetBytes("NORK"));
            UInt32 verId = 0x001;
            UInt32 dirCount = (UInt32)directories.Count - 1;
            UInt32 dirOffset = (UInt32)(fullFileData.Count + 24 + 32 + fileSize);
            UInt32 fileCount = (UInt32)files.Count;
            UInt32 fileOffset = (UInt32)(fullFileData.Count + 24 + 32);
            byte[] emptySpace = new byte[32];

            for(int i = 0; i < 32; i++)
            {
                emptySpace[i] = 0;
            }

            BinaryWriter outFile;
            outFile = new BinaryWriter(File.Open(output, FileMode.Create));
            outFile.Write(packId);
            outFile.Write(verId);
            outFile.Write(dirCount);
            outFile.Write(dirOffset);
            outFile.Write(fileCount);
            outFile.Write(fileOffset);
            outFile.Write(emptySpace);
            System.Console.WriteLine("------------------Writing File Data------------------");
            for(int i = 0; i < fullFileData.Count; i++)
            {
                outFile.Write(fullFileData[i]);
            }
            System.Console.WriteLine("------------------Writing File Info------------------");
            for(int i = 0; i < files.Count; i++)
            {
                //TODO: Something here isn't working?
                outFile.Write(files[i].dirIndex);
                outFile.Write(files[i].fileNameSize);
                byte[] fileName = Encoding.ASCII.GetBytes(files[i].fileName);
                outFile.Write(fileName);
                outFile.Write(files[i].dataOffset);
                outFile.Write(files[i].dataSize);
            }
            System.Console.WriteLine("------------------Writing Directory Info------------------");
            //Removes the directory from the directory lists
            for(int i = 1; i < directories.Count; i++)
            {
                directories[i] = directories[i].Replace(directory, "");
                if(directories[i].StartsWith("\\") || directories[i].StartsWith("/"))
                {
                    directories[i] = directories[i].Substring(1);
                }
            }

            for(int i = 1; i < directories.Count; i++)
            {
                byte dirSize = (byte)directories[i].Length;

                outFile.Write(dirSize);
                outFile.Write(Encoding.ASCII.GetBytes(directories[i]));
            }

            outFile.Close();

        }
    }

}
