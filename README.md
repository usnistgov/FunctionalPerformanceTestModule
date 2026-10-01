# 1. Software or Data description
The Functional Performance Test Module (FPTM) is a tool developed at NIST and is used to perform passive or active testing and analysis of HVAC 
systems by interacting in realtime with BACnet systems. The FPTM uses a series of commands to drive the mechanical systems into its various normal 
modes of operation. Rules may be used to determine whether the system response was appropriate.
The FPTM is being developed in coordination with the efforts of ASHRAE SGPC 36 High Performance Sequences of Operation for HVAC Systems, and ASHRAE
SPC 236 Method of Test for Control Programming Conformance with HVAC Sequences of Operation.
The FPTM is currently at DRAFT stage and is actively being worked on.
Instructions for installing and operating the FPTM can be found in the document "Directions for using FPTM software.rev4.docx" or a newer version
as indicated by the revision value.

# 2. Measurement Uncertainty
The FPTM does not take measurements of data. It does read measured values from controllers or other sources such as configuration files or user entered
data. The measurement uncertainty of values in the FPTM depends on the uncertainty of the source of the data. In some cases a number may be rounded for 
display purposes, but the underlying value is not modified.

# 3. Description of Files
There are two directories available, code and doc. The files in the code directory are the source files for the FPTM and are only useful for those who want
to modify the functionality of the FPTM. These files are not needed for users who do not want to modify the source code. The files in the doc directory are
instructions on use of the FPTM and should be useful for all users. Binary files for the FPTM are available as a release (link usually on the right side of
the window). This is the version most people will find useful. A release version will be available when BETA testing is complete.

# 4. Contact Information
The FPTM is developed by:

Michael A. Galler - mikeg@nist.gov
NIST Engineering Laboratory
Building Energy and Environment Division
Mechanical Systems and Controls Group

# 5. Data Use Notes
This data is publicly available according to the NIST statements of copyright, fair use and licensing; see
https://www.nist.gov/director/copyright-fair-use-and-licensing-statements-srd-data-and-software

Product Disclaimer:
Certain equipment, instruments, software, or materials are identified in this software in order to 
specify the experimental procedure adequately.  Such identification is not intended to imply 
recommendation or endorsement of any product or service by NIST, nor is it intended to imply 
that the materials or equipment identified are necessarily the best available for the purpose.

You may cite the use of this data as follows:
Michael A. Galler (2026), The Functional Performance Test Module, Version 1.0.0,
National Institute of Standards and Technology,
https://doi.org/10.18434/mds2-4232 (Accessed: [give download date])
