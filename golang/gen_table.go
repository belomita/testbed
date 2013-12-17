package main

import (
	"fmt"
	"math/rand"
	"os"
	"strings"
	"time"
)

const (
	STRING_LEN int = 8
	LINE_NUM   int = 10
	COL_NUM    int = 6
)

func main() {
	t1 := time.Now()
	slice := make([]byte, STRING_LEN)
	cols := make([]string, COL_NUM)
	//lines := make([]string, LINE_NUM)
	for line := 1; line < LINE_NUM; line++ {
		for col := 0; col < COL_NUM; col++ {
			for index := 0; index < STRING_LEN; index++ {
				slice[index] = byte(rand.Intn(3)) + 'a'
			}
			cols[col] = string(slice)
		}
		//lines[line] = strings.Join(cols, "\t")
		fmt.Println(strings.Join(cols, "\t"))
	}

	t2 := time.Now()
	fmt.Fprintf(os.Stderr, "Time: %v.", t2.Sub(t1))
}
