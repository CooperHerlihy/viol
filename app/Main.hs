module Main (main) where

main :: IO  ()
main = print (parseCode "2 3 + 1 1 + 30 - *")
-- main = print (parseCode "* + 2 3 - + 1 1 30")

parseCode :: String -> Double
parseCode code = parse code []
    where

    parse [] [res] = res
    parse codeData stack = parse rest (parseNext next stack)
        where
        (next, rest) = getNext codeData

    getNext [] = ([], [])
    getNext (' ':xs) = ([], xs)
    getNext ('\n':xs) = ([], xs)
    getNext (x:xs) = (x:next, rest)
        where
        (next, rest) = getNext xs

    parseNext "+" (b:a:stack) = (a + b):stack
    parseNext "-" (b:a:stack) = (a - b):stack
    parseNext "*" (b:a:stack) = (a * b):stack
    parseNext "/" (b:a:stack) = (a / b):stack

    parseNext num@('0':_) stack = (parseNum num):stack
    parseNext num@('1':_) stack = (parseNum num):stack
    parseNext num@('2':_) stack = (parseNum num):stack
    parseNext num@('3':_) stack = (parseNum num):stack
    parseNext num@('4':_) stack = (parseNum num):stack
    parseNext num@('5':_) stack = (parseNum num):stack
    parseNext num@('6':_) stack = (parseNum num):stack
    parseNext num@('7':_) stack = (parseNum num):stack
    parseNext num@('8':_) stack = (parseNum num):stack
    parseNext num@('9':_) stack = (parseNum num):stack

    parseNext _ _ = undefined

    parseNum [] = undefined
    parseNum "0" = 0.0
    parseNum "1" = 1.0
    parseNum "2" = 2.0
    parseNum "3" = 3.0
    parseNum "4" = 4.0
    parseNum "5" = 5.0
    parseNum "6" = 6.0
    parseNum "7" = 7.0
    parseNum "8" = 8.0
    parseNum "9" = 9.0
    parseNum (x:xs) = parseNum [x] * 10.0 ^^ (length xs) + parseNum xs

