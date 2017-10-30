# -*- coding: utf-8 -*-
"""
program: parse_mdf_class.py

Reads mdf binary file

mdf format:
  identifier: 6 bytes
  NumHeaderEntries: 2 bytes
  HeaderEntry[NumHeaderEntries] (array of header entries)
      * Each HeaderEntry is composed by 32 bytes
"""
#!/usr/bin/python
#%% import
import struct
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.interpolate import interp1d
import collections

class ParseMDF:
    
    isDebug = True
    
    def __init__(self):
        
        if(self.isDebug):
            print "Ititializing variables"
        
        # Variable that will store all binary data as a stream
        self.data_stream = ""
        
        # Pandas DataFrame
        self.data_frame = ""
        
        # Create a data Structure that will contain all MDF binary data
        self.CodaData = collections.OrderedDict()
        self.CodaData['TextComments'] = ""
        self.CodaData['Date'] = ""
        self.CodaData['MarkerPosition'] = []
        self.CodaData['AnalogueForce'] = []
        self.CodaData['AnalogueEMG'] = []
        self.CodaData['InView'] = []
        self.CodaData['EventData'] = []
        self.CodaData['SamplingRatesMarkers'] = []
        self.CodaData['SamplingRateForce'] = []
        self.CodaData['SamplingRatesEMG'] = []
        self.CodaData['SamplingRatesEvent'] = []
        self.CodaData['TimeScaleCursor'] = []
        self.CodaData['GraphCursor'] = []
        self.CodaData['MarkerPositionResolution'] = []
        self.CodaData['HardwareMarkerAdquired'] = []
        self.CodaData['DataDisplayZoom'] = []
        self.CodaData['TimeScaleForZoom'] = []
        self.CodaData['GaitCicle'] = []
        self.CodaData['AnalogueForceResolution'] = []
        self.CodaData['AnalogueEMGResolution'] = []
        self.CodaData['ForcePlateConstant'] = []
        self.CodaData['ForcePlatePosition'] = []
        self.CodaData['MarkerNames'] = []
        self.CodaData['AnalogueForceNames'] = []
        self.CodaData['AnalogueEMGNames'] = []
        self.CodaData['PatientID'] = []
        self.CodaData['PatientClassification'] = []
        self.CodaData['PatientData'] = []
        self.CodaData['PatientSegmentData'] = []
        self.CodaData['PatientDataNames'] = []
        self.CodaData['VideoData'] = []
        self.CodaData['ForcePlateTypes'] = []
        self.CodaData['ForcePlateFlags'] = []
        self.CodaData['ForcePlateCoP'] = []
        self.CodaData['ADC_EMG_ChannelNumbers'] = []
        self.CodaData['MarkerIntensity'] = []
        self.CodaData['CalculatedData_ChannelNames'] = []
        self.CodaData['CalculatedData_DataType'] = []
        self.CodaData['CalculatedData_Resolution'] = []
        self.CodaData['CalculatedMarkerPosition'] = []
        self.CodaData['nMarkerSamples'] = 0
        self.CodaData['nADCSamples'] =  0
        
        # Copy keys to a new variables
        self.TypeCode = self.CodaData.keys()
    
    #%%****************************************************************************     
    #                       Parse MDF file, and extract data
    # input:  MDF format filename
    # output: Relevant data in Pandas DataFrame format 
    #         All Parsed data in a Dictionary  
    #****************************************************************************         
    def parse_file(self, filename):
        
        # Open file        
        self.open_file(filename)
        
        # Get Identifier
        [name, version] = self.get_identifier()
    
        # Get NumHeaderEntries
        NumHeaderEntries = self.get_num_header_entries()
        
        header_lst = self.get_header_entries(NumHeaderEntries)
        
        # Parse Header Entries
        self.parse_header_entries(header_lst)
        
        # Process data. 
        # Resample Markers data to get the same size of ADC
        # Resample Visivilituy Flags
        self.processData()
        
        #% Dump data to Pandas DataFrame format
        self.data_frame = self.dump2DataFrame()
        
        return self.data_frame, self.CodaData

        
    #%% Open file as a character stream, and return it
    def open_file(self, filename):

        fd = open(filename, "rb")
        self._data_stream = fd.read()    
    
    #%% Store Pandas DataFrame to a file
    def store_data(filename, data_frame):
        
        print "Storing data. This might take time..."

        data_frame.to_csv(filename, index=False)

        print "Data stored."
   #%%****************************************************************************
    #                          Get Identifier
    # The Identifier is always 6 bytes long, and it is defined in the first 6 bytes
    # uchar[4] name 
    # uint8[2] version
    #******************************************************************************
    def get_identifier(self):
    
        identifier = []
        identifier = {'name': [], 'version':[] }

        name = self.data_stream[0:4]
        version = str(ord(self.data_stream[4])) + '.' + str(ord(self.data_stream[5])) # ord: get a interger value from a byte
          
        identifier['name'] = name
        identifier['version'] = version
        
        # copy identifier to global Coda data structure
        self.CodaData['Identifier'] = identifier
          
        if (self.isDebug):
            print "Identifier: "
            print "  name:    %s" %identifier['name']
            print "  version: %s" %identifier['version']

        return name, version
    
    #%%****************************************************************************
    #                  Get Number of Header Entries
    # The number of Header Entries is always represented by byte[6] and byte[7] as:
    # uint16 NumHeaderEntries
    #******************************************************************************
    def get_num_header_entries(self):
        
        NumHeaderEntries  = struct.unpack('<h', self.data_stream[6] + self.data_stream[7])[0] 
    
        if (self.isDebug):
            print "NumHeaderEntries: %d" % (NumHeaderEntries)
    
        return NumHeaderEntries;