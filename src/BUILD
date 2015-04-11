cc_library (
  name = 'epoll_event',
  srcs = [
    'event_worker.cc',
    'log.cc',
    'poller.cc',
    'signal.cc',
  ],

  incs = [
    #'../../index_protocol',
    #'../../thirdparty/publiclib/comm',
    #'../../thirdparty/snappy',
    '../../thirdparty/publiclib/wbl_oss/include', 
  ],

  link_all_symbols = 'false',
)

# cc_binary (
#  name = 'indexer_bin',
#   srcs = [
#     'event_worker.cc',
#     'log.cc',
#     'poller.cc',
#     'signal.cc',
#  ],

#   incs = [
     #'../include',
     #'../util',
     #'../../index_protocol',
     #'../../thirdparty/snappy',
     #'../../thirdparty/publiclib/comm',
#     '../../thirdparty/publiclib/wbl_oss/include', 
#  ],

#   deps = [
     #'#expat',
#     '#pthread',
     #'#z',
     #'//index_protocol:index_protocol',
     #'//retrieval/util:util',
     #'//thirdparty:mpcomm',
#     '//thirdparty:wbl',
#     #'//thirdparty:protobuf',
#     #'//thirdparty:snappy',
#     #'//thirdparty:dnscli_64',
#   ],
#
#   link_all_symbols = 'yes',
#)
