#!/usr/bin/env python
# -*- coding: utf-8 -*-
import json
import os
	
def main():
	tests = {
		'ID test (tt0092099)' : 4,
		'ID test (tt0076759)' : 4,
		'ID test (tt1727824)' : 4,
		'ID test (tt4123430)' : 4,
		'ID test (tt0209144)' : 4,
		'ID test (tt0047478)' : 4,
		'ID test (tt0211915)' : 4,
		'ID test (tt0062229)' : 4,
		'ID test (tt0118530)' : 4,
		'ID test (tt0000000) - Does not exist' : 4,
		'Name test (Top Gun)' : 10,
		'Name test (Titanic)' : 10,
		'Name test (Dracula)' : 10,
		'Name test (Batman)' : 10,
		'Name test (Blah) - Does not exist' : 10
	}
	with open('summary.json') as f:
		data = json.load(f)
		if not data:
			return

		with open(os.getenv('GITHUB_STEP_SUMMARY'), 'a', encoding="utf-8") as outFile:
			points = 0
			outFile.write("## Grade Report\n")
			outFile.write("<table>\n")
			outFile.write("<thead><tr><td><b>Result</b></td><td><b>Test</b></td>")
			outFile.write("<td><b>Points</b></td><td><b>Earned</b></td><td><b>Details</b></td></tr></thead>\n")
			
			for test in tests.keys():
				errorFound = False

				for error in data["Run"]["Failures"]:
					if error["Source"]["Name"] == test:
						outFile.write("<tr><td>❌ FAIL</td>\n")
						outFile.write("<td>" + test + "</td>\n")
						outFile.write("<td>" + str(tests[test]) + "</td>\n")
						outFile.write("<td>" + str(0) + "</td>\n")
						outFile.write("<td><details closed><summary>Failure Info</summary>\n")
						outFile.write("<pre>\nFAILED: " + error["Error"]["Test"])
						outFile.write("\nMESSAGE: " + error["Error"]["Message"] + "</pre></details></td>\n")
						outFile.write("</tr>")
						errorFound = True
						break

				if not errorFound:
					outFile.write("<tr><td>✅ PASS</td>\n")
					outFile.write("<td>" + test + "</td>\n")
					outFile.write("<td>" + str(tests[test]) + "</td>\n")
					outFile.write("<td>" + str(tests[test]) + "</td>\n")
					outFile.write("<td></td>\n")
					outFile.write("</tr>")
					points += tests[test]

			outFile.write("<tr><td colspan='2'><b>TOTAL</b></td>")
			outFile.write("<td><b>90</b></td>")
			outFile.write("<td><b>" + str(points) + "</b></th><td></td></tr>")
			outFile.write("</table>\n")

			if points == 90:
				outFile.write("\n## 🔥 All test cases passed!! 🔥\n\n")
				outFile.write("You received all 90 points for the unit tests.\n\n")
			else:
				outFile.write("\n## 🚨🚨 Some test cases failed!! 🚨🚨\n\n")
				outFile.write("You received " + str(points) + " out of 90 points.\n\n")

			outFile.write("Keep in mind you still need to check for warnings, and there may be additional assignment-specific points.\n")
	
if __name__ == '__main__':
	main()
