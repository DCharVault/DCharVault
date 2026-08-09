import type {SidebarsConfig} from '@docusaurus/plugin-content-docs';

const sidebars: SidebarsConfig = {
  docs: [
    {
      type: 'category',
      label: 'Overview',
      collapsible: false,
      items: [
        'intro',
        'getting-started',
      ],
    },
    {
      type: 'category',
      label: 'Architecture',
      collapsible: false,
      items: [
        'ARCHITECTURE',
      ],
    },
    {
      type: 'category',
      label: 'API Reference',
      collapsible: false,
      items: [
        'CORE_MODELS',
        'VIEW_MODELS',
      ],
    },
    {
      type: 'category',
      label: 'Guides',
      collapsible: false,
      items: [
        'error-handling',
      ],
    },
  ],
};

export default sidebars;