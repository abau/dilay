{- This file is part of Dilay
 - Copyright © 2015-2017 Alexander Bau
 - Use and redistribute under the terms of the GNU General Public License
 -}

import Control.Exception (assert)
import Control.Monad (forM_)
import Data.Char (toLower)
import Data.Function (on)
import Data.List (intercalate,nubBy,sortBy)

data Configuration = Configuration 
  { base :: Int
  , vertices :: [Bool]
  , vertexIndices :: [Int]
  , nonManifoldConfig :: Bool
  } 
  deriving Show

set :: Int -> a -> [a] -> [a]
set 0 x (y:ys) = x:ys
set i x (y:ys) = y:(set (i - 1) x ys)

setList :: [Int] -> a -> [a] -> [a]
setList is value xs = foldl (\xs i -> set i value xs) xs is

baseConfigurations :: [Configuration]
baseConfigurations = 
  [ go 0  []                       []                                             False
  , go 1  [4]                      [[2, 6, 7]]                                    False
  , go 2  [4, 5]                   [[2, 5, 7, 10]]                                False
  , go 3  [4, 7]                   [[2, 6, 7], [9, 10, 11]]                       False
  , go 4  [3, 4]                   [[2, 6, 7], [3, 4, 11]]                        False

  , go 5  [0, 1, 5]                [[1, 2, 4, 6, 10]]                             False
  , go 6  [3, 4, 5]                [[2, 5, 7, 10], [3, 4, 11]]                    False
  , go 7  [3, 5, 6]                [[7, 8, 9], [3, 4, 11], [5, 6, 10]]            False
  , go 8  [0, 1, 4, 5]             [[1, 4, 7, 10]]                                False
  , go 9  [0, 1, 2, 4]             [[3, 4, 5, 6, 7, 8]]                           False

  , go 10 [1, 3, 4, 6]             [[2, 6, 8, 9], [0, 3, 5, 11]]                  False
  , go 11 [0, 1, 3, 4]             [[1, 3, 5, 6, 7, 11]]                          False
  , go 12 [0, 1, 5, 6]             [[7, 8, 9], [1, 2, 4, 6, 10]]                  False
  , go 13 [1, 2, 4, 7]             [[2, 6, 7], [9, 10, 11], [1, 3, 8], [0, 4, 5]] False
  , go 14 [0, 1, 2, 5]             [[2, 3, 4, 6, 8, 10]]                          False

  , go 15 [0, 1, 2, 4, 7]          [[3, 4, 5, 6, 7, 8], [9, 10, 11]]              False
  , go 16 [0, 1, 2, 6, 7]          [[2, 5, 7, 10], [3, 4, 11]]                    True
  , go 17 [2, 3, 4, 6, 7]          [[1, 2, 4, 6, 10]]                             False
  , go 18 [0, 1, 2, 5, 6, 7]       [[2, 6, 7], [3, 4, 11]]                        False
  , go 19 [0, 1, 2, 3, 5, 6]       [[2, 6, 7], [9, 10, 11]]                       True

  , go 20 [0, 1, 2, 3, 6, 7]       [[2, 5, 7, 10]]                                False
  , go 21 [0, 1, 2, 3, 5, 6, 7]    [[2, 6, 7]]                                    False
  , go 22 [0, 1, 2, 3, 4, 5, 6, 7] []                                             False
  ]
  where
    go base vertices vertexIndices nonManifold = 
      Configuration base (setList vertices True $ replicate 8 False)
                    (mkVertexIndices 0 vertexIndices) nonManifold

    mkVertexIndices :: Int -> [[Int]] -> [Int]
    mkVertexIndices vertexIndex [] = replicate 12 (-1)
    mkVertexIndices vertexIndex (edgeGroup:edgeGroups) = setList edgeGroup vertexIndex rest
      where
        rest = mkVertexIndices (vertexIndex + 1) edgeGroups


allConfigurations :: [Configuration]
allConfigurations = sortBy (sort `on` vertices)
                  $ nubBy ((==) `on` vertices)
                  $ concat
                  $ do
  x <- [id, rotateX, rotateX . rotateX, rotateX . rotateX . rotateX]
  y <- [id, rotateY, rotateY . rotateY, rotateY . rotateY . rotateY]
  z <- [id, rotateZ, rotateZ . rotateZ, rotateZ . rotateZ . rotateZ]

  return $ map (($) (x . y . z)) baseConfigurations

  where
    rotateX (Configuration base [v0, v1, v2, v3, v4, v5, v6, v7]
                           [e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11] nonManifold)

      = Configuration base [v4, v5, v0, v1, v6, v7, v2, v3]
                      [e6, e2, e7, e0, e5, e10, e9, e8, e1, e3, e11, e4] nonManifold


    rotateY (Configuration base [v0, v1, v2, v3, v4, v5, v6, v7]
                           [e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11] nonManifold)

      = Configuration base [v1, v5, v3, v7, v0, v4, v2, v6]
                      [e5, e4, e0, e11, e10, e6, e2, e1, e3, e8, e7, e9] nonManifold


    rotateZ (Configuration base [v0, v1, v2, v3, v4, v5, v6, v7]
                           [e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11] nonManifold)

      = Configuration base [v2, v0, v3, v1, v6, v4, v7, v5]
                      [e1, e3, e8, e4, e0, e2, e7, e9, e11, e10, e6, e5] nonManifold

    sort xs ys = go (reverse xs) (reverse ys)
      where
        go (False:xs) (True:ys)  = LT
        go (True:xs)  (False:ys) = GT
        go (_:xs)     (_:ys)     = go xs ys

main :: IO ()
main = assert (length allConfigurations == 256)
     $ printBases >> printNonManifold >> printVertexIndices
  where
    printBases = putStrLn $ toList (show . base) allConfigurations

    printNonManifold = putStrLn $ toList (map toLower . show . nonManifoldConfig) allConfigurations

    printVertexIndices =
      putStrLn $ toList (\c -> toList show (vertexIndices c) ++ "\n") allConfigurations

    toList xToString xs = unwords $ "{" :
                                  [ intercalate ", " $ map xToString xs ]
                                  ++ ["}"]
