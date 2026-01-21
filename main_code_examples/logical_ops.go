package main

import "fmt"

func main() {
    var first bool
    var second bool

    fmt.Scan(&first, &second)

    fmt.Print("bool_and ", first && second, "\n")
    fmt.Print("bool_or ", first || second, "\n")
    fmt.Print("bool_not_first ", !first, "\n")
    fmt.Print("bool_eq ", first == second, "\n")
    fmt.Print("bool_ne ", first != second, "\n")
}
