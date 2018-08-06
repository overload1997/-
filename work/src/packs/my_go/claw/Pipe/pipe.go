package main

import (
	"bytes"
	"fmt"
	"os/exec"
)

func main() {
	/*cmd0 := exec.Command("echo", "This is my first command!\nThis is another line!")
	out, err := cmd0.StdoutPipe()
	if err != nil {
		fmt.Printf("%s\n", err)
		return
	}
	if err := cmd0.Start(); err != nil {
		fmt.Printf("%s\n", err)
	}
	str := []byte{}
	length, err := out.Read(str)
	fmt.Printf("length:%d\n", length)
	if err != nil {
		fmt.Printf("%s\n", err)
		return
	}
	fmt.Printf("%s", str[:length])*/
	cmd1 := exec.Command("ps", "aux")
	cmd2 := exec.Command("grep", "apipe")
	var outputBuf1 bytes.Buffer
	cmd1.Stdout = &outputBuf1
	if err := cmd1.start(); err != nil {
		fmt.Printf("Error: The first command can not be startup %s\n", err)
		return
	}
	if err := cmd1.wait(); err != nil {
		fmt.Printf("Error:Couldn't wait for the first command:%s\n", err)
		return
	}
}
