{-# OPTIONS_GHC -Wno-compat-unqualified-imports #-}

-- !!! Re-exporting a module whose contents is partially hidden.
module ShouldCompile ( module Data.List ) where

import Data.List hiding ( sort )

