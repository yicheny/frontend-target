import { createSelects } from '../../../../common/utils'
import { createListWithUrl, getDefaultParams } from '../widgetsBiz/Conctract'
import { createTabContent, SEvDate, STradingBooks, SContractIds, SCounterPartyIds } from '../widgets/components'

export default createTabContent({
  List: createListWithUrl('/valuation/contract/searchOnParty'),
  Selects: createSelects([SEvDate, STradingBooks, SContractIds, SCounterPartyIds]),
  getDefaultParams,
})
