cc_binary (
  name = 'epoll_event_test',
  srcs = [
    'epoll_event_test.cc',
  ],

  incs = [
    '../src/',
    '../../../thirdparty',
    '../../thirdparty/publiclib/wbl_oss/include', 
  ],

  deps = [
    '//epoll_event_processer/src:epoll_event',
    '#pthread',
    '//thirdparty:wbl',
    '//thirdparty:gtest',
    '//thirdparty:gtest_main',
  ],

  link_all_symbols = 'yes',
)
