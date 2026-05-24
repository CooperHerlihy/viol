module Main (main) where

main :: IO  ()
main = print (execCode "+ / 2 3 * / 1 2 - 13 8")

execCode :: String -> Double
execCode = execAst . parseCode

execAst :: AstNode -> Double

execAst (AstAdd [a, b]) = execAst a + execAst b
execAst (AstAdd _) = undefined

execAst (AstSub [a, b]) = execAst a - execAst b
execAst (AstSub _) = undefined

execAst (AstMul [a, b]) = execAst a * execAst b
execAst (AstMul _) = undefined

execAst (AstDiv [a, b]) = execAst a / execAst b
execAst (AstDiv _) = undefined

execAst (AstValue v) = v

execAst AstEmpty = undefined

parseCode :: String -> AstNode
parseCode = parseTokens . tokenizeCode

parseTokens :: [Token] -> AstNode

parseTokens tokens = tree
    where
    (tree, _) = parseSome tokens

    parseSome [] = (AstEmpty, [])

    parseSome (TokenAdd:ts) = (AstAdd args, rest)
        where (args, rest) = parseCount 2 ts

    parseSome (TokenSub:ts) = (AstSub args, rest)
        where (args, rest) = parseCount 2 ts

    parseSome (TokenMul:ts) = (AstMul args, rest)
        where (args, rest) = parseCount 2 ts

    parseSome (TokenDiv:ts) = (AstDiv args, rest)
        where (args, rest) = parseCount 2 ts

    parseSome ((TokenNum num):ts) = (AstValue num, ts)

    parseCount :: Int -> [Token] -> ([AstNode], [Token])
    parseCount 0 ts = ([], ts)
    parseCount count ts = (arg:args, rest)
        where
        (arg, next) = parseSome ts
        (args, rest) = parseCount (count - 1) next

data AstNode
    = AstAdd [AstNode]
    | AstSub [AstNode]
    | AstMul [AstNode]
    | AstDiv [AstNode]
    | AstValue Double
    | AstEmpty

tokenizeCode :: String -> [Token]
tokenizeCode [] = []
tokenizeCode code = codeFirst:(tokenizeCode codeNext)
    where
    (codeFirst, codeNext) = tokenizeNext code

    tokenizeNext [] = undefined

    tokenizeNext (' ':xs) = tokenizeNext xs
    tokenizeNext ('\n':xs) = tokenizeNext xs

    tokenizeNext ('+':xs) = (TokenAdd, xs)
    tokenizeNext ('-':xs) = (TokenSub, xs)
    tokenizeNext ('*':xs) = (TokenMul, xs)
    tokenizeNext ('/':xs) = (TokenDiv, xs)

    tokenizeNext (x:xs)
        | isDigit x = (TokenNum (parseNum num), rest)
        | otherwise = undefined
        where
        (num, rest) = getNum (x:xs)

    getNum [] = ("", "")
    getNum (y:ys)
        | isDigit y = (y:num, rest)
        | otherwise = ("", y:ys)
        where
        (num, rest) = getNum ys

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
    parseNum (x:xs) = (parseNum [x]) * 10.0 ^^ length (xs) + parseNum xs

data Token
    = TokenAdd
    | TokenSub
    | TokenMul
    | TokenDiv
    | TokenNum Double

isDigit :: Char -> Bool
isDigit '0' = True
isDigit '1' = True
isDigit '2' = True
isDigit '3' = True
isDigit '4' = True
isDigit '5' = True
isDigit '6' = True
isDigit '7' = True
isDigit '8' = True
isDigit '9' = True
isDigit _ = False

