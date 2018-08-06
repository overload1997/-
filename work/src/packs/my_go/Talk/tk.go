package Talk

import "fmt"

type People string

type Talk interface {
	Output(content string)
	IfEmpty() bool
}

func (peo *People) Output(content string) {
	fmt.Println(content)
}

func (peo *People) IfEmpty() bool {
	if peo == nil {
		return false
	}
	return true
}
