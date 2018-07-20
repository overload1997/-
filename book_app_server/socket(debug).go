package main

import (
	"bufio"
	"bytes"
	"fmt"
	"net"
	"os"
	"sync"
)

const (
	Conpro    = "tcp"
	Localaddr = "localhost"
	Localhost = "7000"
	Delimiter = '\n'
	CLOSE     = "#close\n"
)

var clientarray = make([]net.Conn, 0, 5)
var clientid int
var listener net.Listener

func init() {
	clientid = 0
	var err error
	listener, err = net.Listen(Conpro, fmt.Sprintf("%s:%s", Localaddr, Localhost))
	if err != nil {
		fmt.Printf("The server started fail.(%s)\n", err)
		return
	}
}

//并发处理客户端

func Getmessage(conn net.Conn, buffer *bufio.Reader) (string, error) {
	ReadBytes, err := buffer.ReadBytes(Delimiter)
	if err != nil {
		return "", err
	}
	return string(ReadBytes), nil
}

var wg sync.WaitGroup

func LocalServer() {
	defer wg.Done()
	defer listener.Close()
	fmt.Printf("The server started successfully.\n")

	go func() {
		for {
			inputReader := bufio.NewReader(os.Stdin)
			input, _ := inputReader.ReadString('\n')
			//fmt.Printf(input)
			if input == CLOSE {
				//fmt.Printf("In the close!")
				for i := 0; i < clientid; i++ {
					clientarray[i].Close()
					wg.Done()
				}
				listener.Close()
				wg.Done()
			}
			if input[0] != '#' {
				fmt.Printf("请输入正确的命令\n")
				continue
			}
			clientindex := 0
			if input[1] <= '9' && input[1] >= '0' {
				var i int = 1
				for i = 1; input[i] <= '9' && input[i] >= '0'; i++ {
					x := input[i] - '0'
					clientindex = clientindex*10 + int(x) //
				}
				if clientindex < 1 || clientindex > clientid {
					fmt.Printf("请输入正确的客户端编号\n")
				} else {
					//fmt.Printf("input:%s", input)
					var buffer bytes.Buffer
					sendstr := input[i:]
					//fmt.Printf(sendstr + "\n")
					buffer.WriteString(sendstr)
					clientarray[clientindex-1].Write(buffer.Bytes())
				}
			}

			var buffer bytes.Buffer
			buffer.WriteString(input)
			//conn.Write(buffer.Bytes())
		}
	}()

	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("The client comes but connected fail!(%s)\n", err)
			break
		}
		wg.Add(1)
		clientarray = append(clientarray, conn)
		fmt.Printf("The client[%d](%s) connected successfully.\n", clientid, conn.RemoteAddr())
		clientid++ //其实这个clientid==len(clientarray)

		go func(conn net.Conn, id int) { //从客户端接收信息
			buffer := bufio.NewReader(conn)
			for {
				mes, err := Getmessage(conn, buffer)
				if err != nil {
					wg.Done()
					fmt.Printf("Recived message from client[%d] fail.(%s)\n", id, err)
					return
				}
				fmt.Printf("Recived message from client[%d] successfully:%s", id, mes)
			}
		}(conn, clientid)

	}
}

/*func Sendmessage(conn net.Conn) (int, error) {
	inputReader := bufio.NewReader(os.Stdin)
	input, _ := inputReader.ReadString('\n')
	var buffer bytes.Buffer
	buffer.WriteString(input)
	return conn.Write(buffer.Bytes())
}*/

func main() {
	wg.Add(1)
	go LocalServer()
	wg.Wait()
}
