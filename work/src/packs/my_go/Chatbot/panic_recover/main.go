package main

import (
	"errors"
	"fmt"
)

func main() {
	outerFunc()
}

func outerFunc() {
	defer func() {
		if p := recover(); p != nil {
			fmt.Printf("Recoverd panic: %s\n", p)
		}
	}()
	innerFunc()
}

func innerFunc() {
	a := [3]int{1, 2, 3}
	fmt.Printf("%d\n", a[1])
	panic(errors.New("An intended fatal error!"))
}
