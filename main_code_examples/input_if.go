package main

import "fmt"

func main() {
    var n int
    fmt.Print("Enter number: ")
    fmt.Scan(&n)

    if n < 0 {
        fmt.Print("neg")
    } else if n == 0 {
        fmt.Print("zero")
    } else {
        fmt.Print("pos")
    }
}
