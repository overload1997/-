package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	fmt.Printf("please input your name:\n")
	in := bufio.NewReader(os.Stdin)
	str, err := in.ReadString('\n')
	if err != nil {
		fmt.Printf("%s", err)
		os.Exit(1) // 1表示异常退出 0表示正常退出
	}
	str = str[:len(str)-1]
	name := str
	//name = strings.ToLower(name)
	fmt.Printf("hi %s!\n", str)
	for {
		fmt.Printf("%s what can I help you?\n", name)
		str, err = in.ReadString('\n')
		str = str[:len(str)-1]
		if err != nil {
			fmt.Printf("%s\n", err)
			os.Exit(1)
		}
		str = strings.ToLower(str)
		switch str {
		case "":
			continue
		case "bye", "nothing":
			fmt.Printf("bye!%s\n", name)
			os.Exit(0)
		default:
			fmt.Printf("I can't catch you.\n")
		}

	}
}
