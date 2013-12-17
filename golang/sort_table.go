package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strings"
	"time"
)

type Row []string
type Table struct {
	Rows []Row
	Keys []int
}

func (t Table) Len() int      { return len(t.Rows) }
func (t Table) Swap(i, j int) { t.Rows[i], t.Rows[j] = t.Rows[j], t.Rows[i] }
func (t Table) Less(i, j int) bool {
	first, second := t.Rows[i], t.Rows[j]
	for _, k := range t.Keys {
		if first[k] != second[k] {
			return first[k] < second[k]
		}
	}
	return false
}

func main() {
	t1 := time.Now()

	filename := os.Args[1]
	keys := []int{0, 1, 2}
	f, _ := os.Open(filename)
	defer f.Close()
	r := bufio.NewReader(f)

	table := new(Table)
	table.Keys = keys
	// Read from file
	for {
		line, err := r.ReadString('\n')
		if err != nil {
			break
		}
		cols := strings.Split(line, "\t")
		table.Rows = append(table.Rows, cols)
	}
	// sort
	sort.Sort(table)
	t2 := time.Now()
	fmt.Fprintf(os.Stderr, "Time: %v.", t2.Sub(t1))
	// output
	for _, v := range table.Rows {
		fmt.Printf(strings.Join(v, "\t"))
	}

}
