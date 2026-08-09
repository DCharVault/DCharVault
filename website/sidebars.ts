import type {SidebarsConfig} from '@docusaurus/plugin-content-docs';

const sidebars: SidebarsConfig = {
  docs: [
    {
      type: 'category',
      label: 'DCharVault Documentation',
      collapsible: false,
      items: [
        'ARCHITECTURE',
        'CORE_MODELS',
        'VIEW_MODELS',
      ],
    },
  ],
};

export default sidebars;