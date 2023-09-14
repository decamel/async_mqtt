import React from 'react'
import { DocsThemeConfig } from 'nextra-theme-docs'

const config: DocsThemeConfig = {
  logo: <span>Async MQTT</span>,
  project: {
    link: 'https://github.com/redboltz/async_mqtt/tree/doc',
  },
  docsRepositoryBase: 'https://github.com/redboltz/async_mqtt/tree/doc',
  footer: {
    text: 'Async MQTT C++ library',
  },
}

export default config