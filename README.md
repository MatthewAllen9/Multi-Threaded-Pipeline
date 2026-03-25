# CIS\*3110 A4 Producer-consumer

### Full Name: First Last 
### Student id: 9999999  

REMOVE EVERYTHING BELOW THIS LINE BEFORE SUBMITTING

We are grading the READMEs anonymously, please do not remove or change the formatting of the name and student id above. Just replace Fist Last with your name and student id with your id.

IMPORTANT: the goal of any README is to make it *easy* for the person who is reading it (could be yourself in the future) to:

- understand what the project is about;

- compile and run the code;

- try common usage scenarios.

*Everything* you write in this README should serve this goal. 

- Do not write too much, but provide enough detail. Assume prior knowledge of programming and basic tools like make, gcc, git, etc. 

- Add subsections and sections to make the information easily searchable;

- Use proper formatting for code/commands, bullet points, paragraphs; 

- Provide inputs/outputs and/or demo script to help the user to see the program in action;

- Some repetition is OK if it makes it easier for the user. E.g. when describing common usage scenarios, you can repeat information that was already listed in earlier sections.

REMOVE EVERYTHING ABOVE THIS LINE BEFORE SUBMITTING


KEEP THE SECTION TITLES BUT DELETE ALL INSTRUCTIONS BEFORE SUBMITTING

## Overview

This section should contain the following information:

[1.] A brief high-level description of the problem in your own words.

[2.] Implementation of the concurrent queue. If you used the implementation from the Chapter 29.3, acknowledge it explicitly. Provide only a basic description of how a queue operates for context. Primary focus should be on how your implementation enables concurrent execution while preventing race conditions. 

[3.] Implementation of the producers and how they signal to other threads that input is finished.

[4.] Implementation of the workers, how they know when to stop and communicate this to consumer threads.

[5.] Implementation of the consumers, how they know when to stop and how they maintain original ordering of the input, while maintaining concurrency.

Do not copy the assignment instructions, do not use LLMs to generate this, use your own words. 

When describing the implementation, refer to the specific synchronization primitives that were used.

Keep this section specific but brief. In total, the overview should not exceed 2,000 characters (including white spaces, newlines, sybsection titles, pseudo-code, etc.).

The README overall should not exceed 4,000 characters.

## Installation

Explain how to compile the code on Linux (optionally on other OSs).

## Usage

Explain how to run the code, what are the parameters, inputs (valid and invalid), assumptions, etc. 

Explain what the code outputs and in what format.

Include several common usage examples in the repo. Show the corresponding outputs. Explain more if needed.

You may also provide a demo script or a set of commands to run the algorithm with different options. 

KEEP THE SECTION TITLES BUT DELETE ALL INSTRUCTIONS BEFORE SUBMITTING