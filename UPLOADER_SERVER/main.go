package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"net"
	"os"
	"time"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("You need to supply a server location!\n")
		return
	}

	listen, err := net.Listen("tcp", os.Args[1])

	if err != nil {
		fmt.Printf("Failed to set up TCP Server!\n")
		return
	}

	fmt.Printf("Server started!\n")

	for {

		conn, err := listen.Accept()

		go func(conn net.Conn) {
			time.Sleep(1000 * time.Millisecond)

			if err != nil {
				fmt.Printf("Failed to accept client!\n")
				listen.Close()
				return
			}

			header := make([]byte, 8)

			read, err := conn.Read(header)

			if err != nil {
				fmt.Printf("Failed to read!\n")
				listen.Close()
				return
			}

			actualData := header[:read]

			reader := bytes.NewBuffer(actualData)

			var fileSize int32
			var filenameSize int32

			binary.Read(reader, binary.LittleEndian, &fileSize)
			binary.Read(reader, binary.LittleEndian, &filenameSize)

			filenameBytes := make([]byte, filenameSize)

			conn.Read(filenameBytes)

			filenameString := string(filenameBytes)

			time.Sleep(1000 * time.Millisecond)

			fileData := []byte{}

			for {
				time.Sleep(250 * time.Millisecond)

				readData := make([]byte, fileSize)
				fileRead, _ := conn.Read(readData)

				if fileRead == 0 {
					break
				}

				for i := 0; i != fileRead; i++ {
					fileData = append(fileData, readData[i])
				}
			}

			file, _ := os.Create(filenameString)

			binary.Write(file, binary.LittleEndian, &fileData)

			file.Close()

			fmt.Printf("Successfully received %s! Size: %d\n", filenameString, fileSize)
		}(conn)
	}

}
