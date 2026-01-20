package main

import "fmt"

func appendExcl(s string) string {
    return s + "!"
}

func main() {
    fmt.Print(appendExcl("go"))
}
