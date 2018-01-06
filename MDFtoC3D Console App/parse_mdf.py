# -*- coding: utf-8 -*-
"""
program: parse_mdf.py

Reads mdf binary file

mdf format:
  identifier: 6 bytes
  NumHeaderEntries: 2 bytes
  HeaderEntry[NumHeaderEntries] (array of header entries)
      * Each HeaderEntry is composed by 32 bytes
"""
#!/usr/bin/python
#%% import
# Is there some reason that import sys was commented out?
import sys
import struct
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.interpolate import interp1d
import collections

# Clear all variables
#sys.modules[__name__].__dict__.clear()

#%% Global variables
tmp = []
text = []
date = []
ind_adc_emg = 0
date = []
e_resolution = 1.84 # From Documentation
resample_factor = 5
isDebug = True
isVerbose = True

#CodaData = {'Identifier': [], 'Date':[], 'MarkerPosition':[], 'Analog':[], 'InView':[], 'EventData':[], 'SamplingRateMarkers':[], 'SamplingRateADC':[], 'SamplingRateEvent':[],'nMarkerSamples': 0, 'nADCSamples': 0}
CodaData = collections.OrderedDict()
CodaData['TextComments'] = ""
CodaData['Date'] = ""
CodaData['MarkerPosition'] = []
CodaData['AnalogueForce'] = []
CodaData['AnalogueEMG'] = []
CodaData['InView'] = []
CodaData['EventData'] = []
CodaData['SamplingRatesMarkers'] = []
CodaData['SamplingRateForce'] = []
CodaData['SamplingRatesEMG'] = []
CodaData['SamplingRatesEvent'] = []
CodaData['TimeScaleCursor'] = []
CodaData['GraphCursor'] = []
CodaData['MarkerPositionResolution'] = []
CodaData['HardwareMarkerAdquired'] = []
CodaData['DataDisplayZoom'] = []
CodaData['TimeScaleForZoom'] = []
CodaData['GaitCicle'] = []
CodaData['AnalogueForceResolution'] = []
CodaData['AnalogueEMGResolution'] = []
CodaData['ForcePlateConstant'] = []
CodaData['ForcePlatePosition'] = []
CodaData['MarkerNames'] = []
CodaData['AnalogueForceNames'] = []
CodaData['AnalogueEMGNames'] = []
CodaData['PatientID'] = []
CodaData['PatientClassification'] = []
CodaData['PatientData'] = []
CodaData['PatientSegmentData'] = []
CodaData['PatientDataNames'] = []
CodaData['VideoData'] = []
CodaData['ForcePlateTypes'] = []
CodaData['ForcePlateFlags'] = []
CodaData['ForcePlateCoP'] = []
CodaData['ADC_EMG_ChannelNumbers'] = []
CodaData['MarkerIntensity'] = []
CodaData['CalculatedData_ChannelNames'] = []
CodaData['CalculatedData_DataType'] = []
CodaData['CalculatedData_Resolution'] = []
CodaData['CalculatedMarkerPosition'] = []
CodaData['nMarkerSamples'] = 0
CodaData['nADCSamples'] =  0
#CodaData = {'TextComments':[], 'Date':[], 
#            'MarkerPosition':[], 'Analog':[], 'InView':[], 'EventData':[], 
#            'SamplingRateMarkers':[], 'SamplingRateADC':[], 'SamplingRateEvent':[],
#            'TimeScale:': [], 'GraphCursor':[],
#            'MarkerResolution':[],
#            'HardwareMarkers':[],
#            'Identifier': [], 'nMarkerSamples': 0, 'nADCSamples': 0}
TypeCode = CodaData.keys()#(['TextComments', 'Date','MarkerPosition','Analog','InView', 'EventData', 'SamplingRateMarkers','SamplingRateADC', 'SamplingRateEvent'])
MarkerLabels = (['M1.X', 'M1.Y', 'M1.Z', 'M2.X', 'M2.Y', 'M2.Z','M3.X', 'M3.Y', 'M3.Z', 'M4.X', 'M4.Y', 'M4.Z','M5.X', 'M5.Y', 'M5.Z', 'M6.X', 'M6.Y', 'M6.Z','M7.X', 'M7.Y', 'M7.Z', 'M8.X', 'M8.Y', 'M8.Z'])
InViewLabels = (['1.V', '2.V', '3.V','4.V', '5.V','6.V','7.V','8.V'])
ADCLabels = (['ADC 1', 'ADC 2','ADC 3','ADC 4','ADC 5','ADC 6','ADC 7','ADC 8'])
for i in range(8, 100):
  TypeCode.append(i)

parseDataList = np.array([2,3,4,7,8,9,10,11,12,13,14,19,34,35])
parseBitwiseList = np.array([5,6])
parseTextList = np.array([22,24,25,26])
#%%****************************************************************************
#                          Get Identifier
# The Identifier is always 6 bytes long, and it is defined in the first 6 bytes
# uchar[4] name 
# uint8[2] version
#******************************************************************************
def get_identifier ( data_stream ):

  identifier = []
  identifier = {'name': [], 'version':[] }
  name = data_stream[0:4]
  version = str( ord( data_stream[4] ) ) + '.' + str( ord( data_stream[5] ) ) # ord: get a interger value from a byte
  
  identifier['name'] = name
  identifier['version'] = version

  # copy identifier to global Coda data structure
  CodaData['Identifier'] = identifier
  
  if isVerbose :
    print
    print "Identifier: "
    print "  name:    %s" %identifier['name']
    print "  version: %s" %identifier['version']
    print

  return name, version

#%%****************************************************************************
#                  Get Number of Header Entries
# The number of Header Entries is always represented by byte[6] and byte[7] as:
# uint16 NumHeaderEntries
#******************************************************************************
def get_num_header_entries( data_stream ):

  NumHeaderEntries  = struct.unpack('<h',data_stream[6]+data_stream[7])[0] 

  if isDebug:
    print
    print "NumHeaderEntries: %d" % (NumHeaderEntries)
    print

  return NumHeaderEntries;

#%%****************************************************************************
#                          Get Header Entries
# Header Format from Documentation
# Each Header Entry is always 32 bytes long
# HEADER { uint16 NumHeaderEntries;
#          HEADER_ENTRY[NumHeaderEntries] uint16 { uint8 DataArrayType;
#                                                  uint4 DataTypeSize;
#                                                  uint4 DataDimensionality;
#                                         uint16 NumDataArrays; } }
#******************************************************************************
def get_header_entries ( data_stream, NumHeaderEntries ):
  
  header_lst = []
  
  for i in range(0, NumHeaderEntries):
  
    header_entry= {'DataArrayType': [], 'DataTypeSize': [],'DataDimensionality': [], 'NumDataArrays': []}
  
    # first Header index is in index = 8
    # whole Header entry contains 4 bytes
    # Identifier always 6 bytes
    identifier_size = 6
  
    # After identifier size, 2 bytes are reserved to set NumHeaderEntries
    num_entry_size = 2  
  
    ind_header_entry = identifier_size + num_entry_size + i * 4
  
    # First Header Entry byte represents DataArrayType
    # convert hexadecimal value in unsigned int type. use '<B'
    header_entry['DataArrayType'] =  struct.unpack( '<B', data_stream[ind_header_entry] )[0]
    
    # Second Header Entry byte represents DataTypeSize and DataDimensionality
    # Both parameters are expressed in uint4 format. This means that both information is enclosed in 1 byte
    # A mask is applied to extact each value
    header_entry['DataTypeSize'] = (struct.unpack('<B',data_stream[ind_header_entry+1])[0] )&int('11110000',2)
    header_entry['DataDimensionality'] = (struct.unpack('<B',data_stream[ind_header_entry+1])[0] )&int('00001111',2)    
    
    # Third and Fourth Header Entry bytes represents NumDataArrays
    h_hex = data_stream[ind_header_entry+2] + data_stream[ind_header_entry+3]
    # convert hexadecimal value in short int type. use '<h'
    header_entry['NumDataArrays'] = struct.unpack('H',h_hex)[0]
    
    header_lst.append(header_entry)

    if isDebug:
      print "Header Entry[%02d]" % (i),
      print "  DataArrayType:      %2d" % (header_entry['DataArrayType']),
      print "  DataTypeSize:       %2d" % (header_entry['DataTypeSize']),
      print "  DataDimensionality: %2d" % (header_entry['DataDimensionality']),
      print "  NumDataArrays:      %2d" % (header_entry['NumDataArrays'])
    
  return header_lst

#%% parse Header_Entry
# Read Header Entry information and process data
# DataArrayType =  0 -> text comments
# DataArrayType =  1 -> Date of data acquisition
# DataArrayType =  2 -> nMarkers
# DataArrayType = 34 -> nEMGADC
def parse_header_entries( data_stream, header_lst ):

  ind_start = 0
  NumHeaderEntries = len(header_lst)
  
  # Identifier always 6 bytes
  identifier_size = 6
  
  # After identifier size, 2 bytes are reserved to set NumHeaderEntries
  header_size = 2  + NumHeaderEntries*4
  
  # Get index for first data.It comes after identifier and header definition
  ind_start = identifier_size + header_size
  
  # Add to the list information number of element that contains 
  for i in range(0, NumHeaderEntries):
    if header_lst[i]['DataTypeSize'] == 16:
      header_lst[i]['number_elements'] = 1
    if header_lst[i]['DataTypeSize'] == 32:
      header_lst[i]['number_elements'] = 2
    if header_lst[i]['DataTypeSize'] == 48:
      header_lst[i]['number_elements'] = 3
      
  # In each iteration loop the index updated for next iteration
  # this method might change in the future with "calculate_index" function  
  for i in range(0, NumHeaderEntries):
    
    # Parse Text Comments
    if header_lst[i]['DataArrayType'] == 0:
      header_entry = header_lst[i]
      [data, ind_end] = extract_data( data_stream, ind_start )
      #[data, ind_end] = parseText( data_stream, ind_start )
      CodaData['TextComments'] = data
      if isDebug:      
        print "Text comments:"
        print "  ind_start: %d" % ind_start
        print "  comments:",
        print data
      else:
        if isVerbose:
          print
          print "Text comments: ",
          print
          print
          print data
      
    # Parse Date of data acquisition
    if header_lst[i]['DataArrayType'] == 1:        
      ind_start = ind_end
      [data, ind_end] = extract_data( data_stream, ind_start ) 
      #[data, ind_end] = parseText( data_stream, ind_start )
      
      date_lst = []
      for j in range(0,3):
        h_hex = data[j*2] + data[j*2+1]
        date = struct.unpack('H',h_hex)[0]
        date_lst.append(date)
      
      CodaData['Date'] = date_lst

      if isDebug:
        print "Date of data acquisiton:"
        print "  ind_start: %d"%ind_start
        print "  ind_end:   %d"%ind_end
        print "  date:      %d/%d/%d" %(date_lst[0],date_lst[1],date_lst[2])

      else:
        if isVerbose:
          print "Date of Acquisition: %d/%d/%d" %(date_lst[0],date_lst[1],date_lst[2])

    # Parse All Data
    if header_lst[i]['DataArrayType'] > 1:
      if isDebug:
        print "Type ID: %d"%header_lst[i]['DataArrayType']
        print "Type   : %s"%TypeCode[header_lst[i]['DataArrayType']]
        print "i: %d"%i
      header_entry = header_lst[i]
        
      # update starting index from previous iteration
      ind_start = ind_end
      if isDebug:
        print "ind_start: %d"%ind_start
        print "ind_end: %d"%ind_end
      # get Number Data Arrays
      NumDataArrays = header_lst[i]['NumDataArrays']
      if isDebug:
        print "NumDataArrays %d"%NumDataArrays

      for j in range(0, NumDataArrays):        
        if isDebug:
          print "Parsing %s %d"%(TypeCode[header_lst[i]['DataArrayType']], j)
        # find if DataArrayType is in the array parseDataList to decide the method of parsing data
        # parse data using parseData function
        result = (parseDataList == header_lst[i]['DataArrayType']).nonzero()
        if (result[0] >= 0):
          #print "using parseData"
          [data_mat, ind_end, nSamples] = parseData ( data_stream, ind_start, header_entry )
          
          # add to CodaData Dictionary usefull information
          if ( header_lst[i]['DataArrayType'] == 2):
            CodaData['nMarkerSamples'] = nSamples
          if ( header_lst[i]['DataArrayType'] == 4):
            CodaData['nADCSamples'] = nSamples
            
          CodaData[TypeCode[header_lst[i]['DataArrayType']]].append(data_mat)
          ind_start = ind_end
          
        result = (parseBitwiseList == header_lst[i]['DataArrayType']).nonzero()
        # parse data using parseBitwise function
        if (result[0] >= 0):
          #print "using parseBitwise"
          [data_mat, ind_end] = parseBitwise ( data_stream, ind_start, header_entry )
          CodaData[TypeCode[header_lst[i]['DataArrayType']]].append(data_mat)
          ind_start = ind_end
      
        result = (parseTextList == header_lst[i]['DataArrayType']).nonzero()
        # parse data using parseBitwise function
        if (result[0] >= 0):
          [data_mat, ind_end] = parseText ( data_stream, ind_start )
          CodaData[TypeCode[header_lst[i]['DataArrayType']]].append(data_mat)
          ind_start = ind_end


#    # Parse 3D Marker Position
#    if header_lst[i]['DataArrayType'] == 2:
#      if isDebug:      
#        print "3D Marker Position"
#      
#      header_entry = header_lst[i]
#      
#      # update starting index from previous iteration
#      ind_start = ind_end
#      print "ind_start: %d"%ind_start
#      print "ind_end: %d"%ind_end
#      # get amount of stored markers
#      nMarkers = header_lst[i]['NumDataArrays']
#      
#      for j in range(0, nMarkers):
#        print "Parsing Marker %d"%j
#
#        [data_mat, ind_end, nSamples] = parseData (ind_start, header_entry)
#
#        if j == 0:
#          CodaData['nMarkerSamples'] = nSamples
#
#        data_mat = signal.resample(data_mat, len(data_mat)*resample_factor)
#
#        CodaData['MarkerPosition'].append(data_mat)
#
#        ind_start = ind_end
#    
#    if header_lst[i]['DataArrayType'] == 3:      
#      if isDebug:      
#        print"Analog Force Data index: %d" %ind_start
#      ind_start = ind_end
#      
#    # Parse Analog EMG data
#    if header_lst[i]['DataArrayType'] == 4:
#      if isDebug:
#        print"Analog EMG Data (uV * e.resolution-1)"
#
#      header_entry = header_lst[i]
#      
#      # update starting index from previous iteration
#      ind_start = ind_end
#      print "ind_start: %d"%ind_start
#      print "ind_end: %d"%ind_end
#
#      # get amount of stored Analogue EMG Data
#      nEMG = header_lst[i]['NumDataArrays']
#      
#      for j in range(0, nEMG):
#        print "Parsing Analog %d"%j
#        [data_mat, ind_end, nSamples] = parseData (ind_start, header_entry)
#        if j == 0:
#          CodaData['nADCSamples'] = nSamples
#        # scale data
#        global data_mat
#        data_mat = data_mat / e_resolution
#        
#        # copy data to global structure
#        CodaData['Analog'].append(data_mat)
#        
#        # update index        
#        ind_start = ind_end          
##      print "ind_start: %d"%ind_start
##      print "ind_end: %d"%ind_end
#    
#    # Parse In-View Flags
#    if header_lst[i]['DataArrayType'] == 5:
#      if isDebug:
#        print"Marker-in-view flags (1 = In View)"
#      
#      header_entry = header_lst[i]
#      # update starting index from previous iteration
#      ind_start = ind_end
#      print "ind_start: %d"%ind_start
#      print "ind_end: %d"%ind_end
#      
#      nMarkers = header_lst[i]['NumDataArrays']      
#      for j in range(0, nMarkers):
#        print "Parsing In-View %d"%j
#        [data_mat, ind_end] = parseBitwise (ind_start, header_entry)
#        CodaData['InView'].append(data_mat)
#        # update index        
#        ind_start = ind_end  
#
#    # Parse Event Data
#    if header_lst[i]['DataArrayType'] == 6:
#      if isDebug:
#        print"Event Data"
#      
#      header_entry = header_lst[i]
#      # update starting index from previous iteration
#      ind_start = ind_end
#      print "ind_start: %d"%ind_start
#      print "ind_end: %d"%ind_end
#      nEvent = header_lst[i]['NumDataArrays']      
#      for j in range(0, nEvent):
#        print "Parsing Event Data %d"%j
#        [data_mat, ind_end] = parseBitwise (ind_start, header_entry)
#        CodaData['EventData'].append(data_mat)
#        # update index        
#        ind_start = ind_end  
#    
#    # Parse Event Data
#    if header_lst[i]['DataArrayType'] == 7:
#      if isDebug:
#        print"Sampling Rate"
#      
#      header_entry = header_lst[i]
#      # update starting index from previous iteration
#      ind_start = ind_end      
#      print "ind_start: %d"%ind_start
#      print "ind_end: %d"%ind_end
#
#      # get amount of stored Analogue EMG Data
#      nMarkers = header_lst[i]['NumDataArrays']
#      
#      for j in range(0, nMarkers):
#        print "Parsing Sampling Rates %d"%j
#        [data_mat, ind_end, nSamples] = parseData (ind_start, header_entry)
#        
#        # copy data to global structure
#        CodaData['SamplingRates'] = data_mat
#        
#        # update index        
#        ind_start = ind_end  
#      
#    
#    if header_lst[i]['DataArrayType'] == 34:
#       #nEMGADC = header_lst[i]['NumDataArrays']
#      pass
  
      
#%% extract data starting from a given index address
# First 2 bytes represents the size of the data to be returned
# output:
#   ind_end: last index of data 
#   data:  extracted data. 
def extract_data ( data_stream, ind_start ):
 
  # calculate size of data to be returned
  # first 2 bytes represents size of data in uint16 format
  size = struct.unpack('H',data_stream[ind_start] + data_stream[ind_start+1] )[0]
  ind_start = ind_start + 2 
  ind_end = ind_start + size -2
  
  # extract data
  #print"Data_start_ind: %d  Data_end_ind: %d"%(ind_start,ind_end)
  data = data_stream[ind_start:ind_end]
#  print "start_ind: %d"%ind_start
#  print "end_ind: %d"%ind_end
  return data, ind_end

#%% extract data starting from a given index address
# First 2 bytes represents the size of the data to be returned
# output:
#   ind_end: last index of data 
#   data:  extracted data. 
def parseText ( data_stream, ind_start ):
 
  # calculate size of data to be returned
  # first 2 bytes represents size of data in uint16 format
  size = struct.unpack('H',data_stream[ind_start] + data_stream[ind_start+1] )[0]
  ind_start = ind_start + 2 
  ind_end = ind_start + size
  
  # extract data
  #print"Data_start_ind: %d  Data_end_ind: %d"%(ind_start,ind_end)
  data = data_stream[ind_start:ind_end]
#  print "start_ind: %d"%ind_start
#  print "end_ind: %d"%ind_end
  return data, ind_end

#%%***************************************************************************
#                           Parse Data 
# extract block of data containing all information about a data set
#
# Data Format from Documentation  
# DATA { DATA_ENTRY[] { uint16        NumElements;
#                       DATA_TYPE     DataArray[NumElements]; }}
#
# example: extract data of one Markers position
#          extract data of one Analog channel
#******************************************************************************
def parseData( data_stream, ind_start, header_entry ):
    
  # calculate size of data to be returned
  # first 2 bytes represents size of data in uint16 format
  NumElements = struct.unpack( 'H', data_stream[ind_start] + data_stream[ind_start+1] )[0]
  #print ("NumElements: %d")%NumElements
  
  # calculate start and end index 
  ind_start = ind_start + 2 # first 2 bytes have been used to get NumElements, data starts after these 2 bytes
  ind_end = ind_start + NumElements * header_entry['DataDimensionality']
  
  data = data_stream[ind_start:ind_end]
    
  dimension = header_entry['DataDimensionality']	
  number_elements = header_entry['number_elements']
  
  # initialize variables
  #if header_entry['DataTypeSize'] == 16:
  if header_entry['DataDimensionality'] == 2:
    option = 0
    size = 2 # data is stored in 2 bytes
    data_mat = np.zeros((NumElements,1))
    data_arr = np.zeros(1)
  elif header_entry['DataDimensionality'] == 12:
    option = 1
    size = 4 # data is stored in 4 bytes    
    data_mat = np.zeros((NumElements,3))
    data_arr = np.zeros(3)
  if (header_entry['DataDimensionality'] == 4):
    option = 2
    size = 4 # data is stored in 4 bytes    
    data_mat = np.zeros((NumElements,1))
    data_arr = np.zeros(1)

  idx_start = 0  # index of starting address
  it = 0   # iterate through data_mat

  for j in range(0,len(data), dimension):
    for i in range(0,number_elements):
      idx_end = idx_start + size
      f_hex = data[idx_start:idx_end]
      # depending data type (float: 4 bytes, or uint16: 2 bytes), unpack has to be done differently
      if option == 0:
        value = struct.unpack('<h',f_hex)[0]
      if option == 1:
        value = struct.unpack('<f',f_hex)[0]
      if option == 2:
        value = struct.unpack('<I',f_hex)[0]
      
      data_arr[i] = value
      
      # update index for next iteration
      idx_start = idx_end
    
    # copy array to matrix
    data_mat[it] = data_arr
    it = it + 1

  return data_mat, ind_end, NumElements

#%%
def parseBitwise( data_stream, ind_start, header_entry ):
  
  # calculate size of data to be returned
  # first 2 bytes represents size of data in uint16 format  
  NumElements = struct.unpack('H',data_stream[ind_start] + data_stream[ind_start+1] )[0]
  
  # calculate start and end index 
  ind_start = ind_start + 2 # first 2 bytes have been used to get NumElements, data starts after these 2 bytes
  ind_end = ind_start + NumElements*2
  
  data = data_stream[ind_start:ind_end]
  
  # convert stream to binary string
  data = ''.join(format(ord(x), 'b') for x in data)
  
  # initialize variable in which bitwise data will be stored
  data_mat = np.zeros(len(data))

  # copy binary string to array
  for i in range(0, len(data_mat)):
    data_mat[i] = data[i]
  
  data_mat = np.resize(data_mat,CodaData['nMarkerSamples'])
  #TODO verify this works ok
  #data_mat = signal.resample(data_mat, len(data_mat)*resample_factor)
  
  return data_mat, ind_end

#%% Process data. 
# remove \x00 character from MarkersNames and AnalogueEMGNames
# ADC signal needs to be multiplied by a factor
# Resample Markers data to get the same size of ADC
# Resample Visivilituy Flags
def processData( data_stream ):

  # remove \x00 character from AnalogueEMGNames
  for i in range(0, len(CodaData['AnalogueEMGNames'])):
    length = len(CodaData['AnalogueEMGNames'][i])
    CodaData['AnalogueEMGNames'][i] = CodaData['AnalogueEMGNames'][i][0:length-1]
  
  # remove \x00 character from MarkersNames
  for i in range(0, len(CodaData['MarkerNames'])):
    length = len(CodaData['MarkerNames'][i])
    CodaData['MarkerNames'][i] = CodaData['MarkerNames'][i][0:length-1]
  
  # convert Markers data from milimeters to meters
  for i in range(0, len(CodaData['MarkerPosition'])):
    CodaData['MarkerPosition'][i] = CodaData['MarkerPosition'][i]/1000
  
  # apply resolution factor to Analogue EMG Data
  for i in range(0, len(CodaData['AnalogueEMG'])):
    CodaData['AnalogueEMG'][i] = CodaData['AnalogueEMG'][i] / (1.84 * 1000.0)

  # Analogue EMG data and Marker data are acquired in different frequency
  # Markers Position and Visibility data have to be resized to Analogue data size
  # convert Markers data from milimeters to meters
  CodaData['MarkerPositionInterp'] = []
  CodaData['InViewInterp'] = []
  for i in range(0,len(CodaData['MarkerPosition'])):
    CodaData['MarkerPositionInterp'].append(np.zeros((len(CodaData['AnalogueEMG'][0]),3)))
    CodaData['InViewInterp'].append(np.zeros(len(CodaData['AnalogueEMG'][0])))
  
  for i in range(0, len(CodaData['MarkerPosition'])):
    x = np.linspace(0, len(CodaData['MarkerPosition'][i])-1, len(CodaData['MarkerPosition'][i]))
    x_new = np.linspace(0, len(CodaData['MarkerPosition'][i])-1, len(CodaData['AnalogueEMG'][i]))
    for j in range(0, 3):
      y = CodaData['MarkerPosition'][i][:,j]
      f = interp1d(x, y)
      CodaData['MarkerPositionInterp'][i][:,j] = f(x_new)
  
  for i in range(0, len(CodaData['InView'])):
    x = np.linspace(0, len(CodaData['InViewInterp'][i])-1, len(CodaData['InView'][i]))
    x_new = np.linspace(0, len(CodaData['InView'][i])-1, len(CodaData['AnalogueEMG'][i]))
    y = CodaData['InView'][i]
    f = interp1d(x, y)
    CodaData['InViewInterp'][i] = np.round(f(x_new))

#%% Dump data to Pandas DataFrame format
def dump2DataFrame():

  frequency = CodaData['SamplingRatesEMG'][0][0][0] # in HZ

  # Create Number and Time Series to dump as first columns  
  number = np.linspace(1,CodaData['nADCSamples'], CodaData['nADCSamples'])
  time = np.linspace(0,CodaData['nADCSamples']/frequency, CodaData['nADCSamples'])
  data_frame = pd.Series(number, name="Number")
  df = pd.Series(time, name="Time")
  data_frame = pd.concat([data_frame, df], axis=1)

  # Dump Marker Position and Visivility    
  for i in range(len(CodaData['MarkerPosition'])):
    label = []
    label.append(CodaData['MarkerNames'][i]+".X")
    label.append(CodaData['MarkerNames'][i]+".Y")
    label.append(CodaData['MarkerNames'][i]+".Z")
    df = pd.DataFrame(CodaData['MarkerPositionInterp'][i], columns=label)
    data_frame = pd.concat([data_frame, df], axis=1)
    label = CodaData['MarkerNames'][i] + ".V"
    df = pd.Series(CodaData['InViewInterp'][i], name=label)
    data_frame = pd.concat([data_frame, df], axis=1)

  # Dump ADC
  for i in range(len(CodaData['AnalogueEMG'])):
       df = pd.Series(CodaData['AnalogueEMG'][i][:,0], name=CodaData['AnalogueEMGNames'][i])
       data_frame = pd.concat([data_frame, df], axis=1)

  return data_frame
  
def storeData(filename, data_frame):
  print "Storing data. This might take time..."
  #header = "CODAmotion Analysis Text\nCodamotion Analysis Text Save of: Acquisition.1\nSample\tTime\tMarkers"
  data_frame.to_csv(filename, index=False)
  print "Data stored."


def plotData(data, labels):
  print "Ploting Data..."
  for i in range(0, len(data)):
    # plot parsed individual EMG data
    plt.figure(i)
    plt.plot(data[i])
    plt.xlabel(labels[0])
    plt.ylabel(labels[1])
    plt.legend(labels[2])


#%%***************************************************************************
#%%   Start Main function
#%%***************************************************************************

# If in debug mode, then be verbose, too.
if isDebug:
  isVerbose = True

#%% Read file
# ToDo filename as an argument or from configuration file
filename = 'data/data.mdf'

print "***********************************************************************"
print
print "Program:  %s" % sys.argv[0]
print "Filename: %s" % filename
print
print "***********************************************************************"
print

fd = open(filename, "rb")
DataStream = fd.read()

#%% Get Identifier
[name, version] = get_identifier( DataStream )

#%% Get NumHeaderEntries
NumHeaderEntries = get_num_header_entries( DataStream )

header_lst = get_header_entries( DataStream, NumHeaderEntries)

#%% Parse Header Entries
parse_header_entries( DataStream, header_lst )

#%% Process data. 
# Resample Markers data to get the same size of ADC
# Resample Visivilituy Flags
processData( DataStream )

#%% Dump data to Pandas DataFrame format
data_frame = dump2DataFrame()


labels = []
labels.append('time(msec)')
labels.append('Marker Position (mm)')
labels.append('Marker Position')
plotData(CodaData['MarkerPosition'],labels)
#plotData(CodaData['Analog'],labels)

# ToDo filename as an argument or from configuration file
storeData("filename.csv", data_frame)

