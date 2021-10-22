import { createListWithUrl, getDefaultParams } from '../widgetsBiz/Position'
import { createTabContent, SContractIds, SCounterPartyIds, SUnderlyingId } from '../widgets/components'
import { createSelects } from '../../../../common/utils'

export default createTabContent({
  List: createListWithUrl(`/valuation/subContract/realTimeOnParty`),
  Selects: createSelects([SUnderlyingId, SContractIds, SCounterPartyIds]),
  getDefaultParams,
})
