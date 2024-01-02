// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("Buffer");

var check="Hello World!";
var hex=Buffer.fromString(check).toHex();
var str=Buffer.fromHex(hex).toString();

if(check!=str){
	throw "test fail";
};
