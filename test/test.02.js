// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

Script.requireExtension("Console");
Script.requireExtension("Buffer");

function check(name, value, expected) {
	if (value !== expected) {
		throw "test fail: " + name + ": got " + value + ", expected " + expected;
	};
};

var b;
var r;

// setLength

b = Buffer(8);
b.setU8(5, 1);
check("setLength shrink", b.setLength(2), 2);
check("setLength shrink length", b.length, 2);
check("setLength shrink size", b.size, 8);
check("setLength extend", b.setLength(6), 6);
check("setLength extend keeps bytes", b.toHex(), "000000000001");
check("setLength size", b.setLength(8), 8);
check("setLength past size", b.setLength(9), 0);
check("setLength past size length", b.length, 8);
check("setLength negative", b.setLength(-1), 0);
check("setLength zero", b.setLength(0), 0);
check("setLength zero length", b.length, 0);

b = Buffer(20);
check("setLength full size", b.setLength(20), 20);
check("setLength full size data", b.toHex(), "0000000000000000000000000000000000000000");

b = Buffer.fromString("Hello");
check("setLength shrink text", b.setLength(2), 2);
check("setLength shrink text data", b.toString(), "He");
check("setLength extend text", b.setLength(5), 5);
check("setLength extend text data", b.toString(), "Hello");
check("setLength fraction", b.setLength(3.7), 3);
check("setLength fraction data", b.toString(), "Hel");
check("setLength NaN", b.setLength(0 / 0), 0);
check("setLength Infinity", b.setLength(1 / 0), 0);
check("setLength invalid keeps length", b.length, 3);

var notBuffer = {};
notBuffer.setLength = Buffer.prototype.setLength;
r = "";
try {
	notBuffer.setLength(1);
} catch (e) {
	r = "" + e;
};
check("setLength on non-buffer", r, "Error: invalid parameter");

// set with fill, String source

b = Buffer(8);
check("set string fill start 0", b.set(0, "ab", 0, 4, true), 2);
check("set string fill start 0 data", b.toHex(), "61620000");

b = Buffer(8);
check("set string fill start > 0", b.set(4, "ab", 0, 10, true), 2);
check("set string fill start > 0 data", b.toHex(), "0000000061620000");

b = Buffer.fromString("xxxxxxxx");
b.setLength(2);
check("set string fill keeps prefix", b.set(4, "ab", 0, 3, true), 2);
check("set string fill keeps prefix data", b.toHex(), "78787878616200");

// set with fill, Buffer source

b = Buffer(8);
check("set buffer fill start 0", b.set(0, Buffer.fromString("ab"), 0, 4, true), 2);
check("set buffer fill start 0 data", b.toHex(), "61620000");

b = Buffer(8);
check("set buffer fill start > 0", b.set(4, Buffer.fromString("ab"), 0, 10, true), 2);
check("set buffer fill start > 0 data", b.toHex(), "0000000061620000");

b = Buffer(16);
b.set(0, Buffer.fromString("ab"), 0, 8, true);
b.set(8, "cd", 0, 8, true);
check("set fixed width fields", b.toHex(), "61620000000000006364000000000000");

// set without fill

b = Buffer(8);
check("set string no fill", b.set(2, "abc", 1, 2), 2);
check("set string no fill data", b.toHex(), "00006263");
check("set clipped at size", b.set(6, "xyz", 0, 3), 2);
check("set start past size", b.set(8, "xyz", 0, 3, true), 0);
check("set source start past end", b.set(0, "xyz", 3, 3, true), 0);
check("set source start past end data", b.toHex(), "0000626300007879");

// number conversion

check("toNumber Buffer(0)", Script.isNaN(Convert.toNumber(Buffer(0))), true);
check("toNumber fromHex empty", Script.isNaN(Convert.toNumber(Buffer.fromHex(""))), true);
check("toNumber empty with size", Script.isNaN(Convert.toNumber(Buffer(4))), true);
check("compare Buffer(0) == 0", Buffer(0) == 0, false);
check("toNumber text", Convert.toNumber(Buffer.fromString("123")), 123);
b = Buffer.fromString("12345");
b.setLength(2);
check("toNumber limited to length", Convert.toNumber(b), 12);
check("multiply", Buffer.fromString("21") * 2, 42);
