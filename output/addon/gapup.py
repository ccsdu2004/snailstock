#!/usr/bin/python
# -*- coding: UTF-8 -*-

def get_version():
	return "1.2.0"

def get_name():
	return "script.gapup"

def get_return_type():
    return "boolean"
	
def get_description():
	return "this script addon for gap up shape"

def evalute_candlestick(code,stocks):
    current = len(stocks)-1
    if current < 1:
		return "false"
    low = stocks[current]['low']
    prevtop = stocks[current-1]['high']
    if low > prevtop:
	    return "true"
    return "false"
    